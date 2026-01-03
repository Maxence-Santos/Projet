// check_moves_basic2.cpp
// Single-file helper that launches Stockfish (UCI), runs perft 1 on a FEN
// and prints a single integer (legal move count). Supports --daemon and --quiet.
#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
static bool g_quiet = false;

static void log_err(const std::string &s) {
	if (!g_quiet) std::cerr << "[DEBUG] " << s << std::endl;
}

static void print_win_error(const char *prefix = nullptr) {
	DWORD err = GetLastError();
	LPSTR buf = nullptr;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				   NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buf, 0, NULL);
	std::string msg = buf ? buf : "(no message)";
	if (buf) LocalFree(buf);
	std::stringstream ss;
	if (prefix) ss << prefix << ": ";
	ss << "Win32 error " << err << " (0x" << std::hex << err << ") - " << msg;
	log_err(ss.str());
}

// Write command (must include trailing '\n')
static bool write_cmd(HANDLE h, const std::string &cmd) {
	DWORD written = 0;
	if (!WriteFile(h, cmd.c_str(), (DWORD)cmd.size(), &written, NULL)) {
		print_win_error("WriteFile failed");
		return false;
	}
	return written == cmd.size();
}

// Read available data from pipe until timeout_ms expires. Returns collected string.
static std::string read_available(HANDLE h, DWORD timeout_ms) {
	std::string out;
	DWORD start = GetTickCount();
	char buf[4096];
	while (GetTickCount() - start < timeout_ms) {
		DWORD avail = 0;
		if (!PeekNamedPipe(h, NULL, 0, NULL, &avail, NULL)) {
			print_win_error("PeekNamedPipe failed");
			return out;
		}
		if (avail == 0) { Sleep(10); continue; }
		DWORD toread = (DWORD)std::min<size_t>(avail, sizeof(buf) - 1);
		DWORD read = 0;
		if (!ReadFile(h, buf, toread, &read, NULL)) {
			if (GetLastError() == ERROR_BROKEN_PIPE) break;
			print_win_error("ReadFile failed");
			break;
		}
		if (read == 0) { Sleep(10); continue; }
		out.append(buf, read);
	}
	return out;
}

// Effectue perft 1 sur la FEN donnée et analyse "Nodes searched: N".
static int do_perft(HANDLE child_in_write, HANDLE child_out_read, const std::string &fen_in) {
	int moves_local = 0;
	std::string poscmd_local = "position fen " + fen_in + "\n";
	if (!write_cmd(child_in_write, poscmd_local)) return moves_local;
	if (!write_cmd(child_in_write, "go perft 1\n")) return moves_local;
	std::string out_local = read_available(child_out_read, 5000);
	size_t p = out_local.find("Nodes searched: ");
	if (p != std::string::npos) {
		size_t e = out_local.find('\n', p);
	size_t start = p + strlen("Nodes searched: ");
	std::string num = out_local.substr(start, (e == std::string::npos) ? std::string::npos : e - start);
	try { moves_local = std::stoi(num); } catch (...) { moves_local = 0; }
	} else {
		log_err("Perft output did not contain 'Nodes searched'");
	}
	return moves_local;
}

int main(int argc, char **argv) {
	// Parse arguments first to set g_quiet before any logging
	bool daemon_mode = false;
	std::string fen;
	for (int i = 1; i < argc; ++i) {
		std::string a = argv[i];
		if (a == "--daemon") daemon_mode = true;
		else if (a == "--quiet") g_quiet = true;
		else if (fen.empty()) fen = a;
	}
	
	// Chemin du moteur depuis la macro CMake STOCKFISH_PATH, sinon chemin relatif par défaut.
	std::string engine_path;
#ifdef STOCKFISH_PATH
	engine_path = STOCKFISH_PATH;
#else
	engine_path = "stockfish\\stockfish-windows-x86-64-avx2.exe";
#endif
	
	// Essayer plusieurs chemins relatifs pour trouver Stockfish
	std::vector<std::string> paths_to_try = {
		engine_path,
		"..\\..\\stockfish\\stockfish-windows-x86-64-avx2.exe",  // depuis build/Release
		"stockfish\\stockfish-windows-x86-64-avx2.exe",           // depuis la racine
		".\\stockfish\\stockfish-windows-x86-64-avx2.exe"         // relatif au dossier courant
	};
	
	const char *engine = nullptr;
	for (const auto& path : paths_to_try) {
		if (GetFileAttributesA(path.c_str()) != INVALID_FILE_ATTRIBUTES) {
			engine = path.c_str();
			if (!g_quiet) log_err("Found Stockfish at: " + path);
			break;
		}
	}
	
	if (!engine) {
		std::cerr << "ERROR: Stockfish executable not found!\n";
		std::cerr << "Tried the following paths:\n";
		for (const auto& path : paths_to_try) {
			std::cerr << "  - " << path << "\n";
		}
		std::cerr << "\nPlease ensure stockfish-windows-x86-64-avx2.exe is in the 'stockfish' folder.\n";
		std::cerr << "Press any key to close this window...\n";
		system("pause");
		return 1;
	}
	
	for (int i = 1; i < argc; ++i) {
		std::string a = argv[i];
		if (a == "--daemon") daemon_mode = true;
		else if (a == "--quiet") g_quiet = true;
		else if (fen.empty()) fen = a;
	}

	if (!daemon_mode && fen.empty()) {
		std::cerr << "Usage: " << argv[0] << " [--daemon] [--quiet] <FEN>\n";
		std::cerr << "Press any key to close this window...\n";
		system("pause");
		return 1;
	}
	// Créer des pipes anonymes pour la sortie standard et l'entrée standard de l'enfant
	SECURITY_ATTRIBUTES sa{};
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	HANDLE child_out_read = NULL, child_out_write = NULL;
	HANDLE child_in_read = NULL, child_in_write = NULL;
	if (!CreatePipe(&child_out_read, &child_out_write, &sa, 0)) { print_win_error("CreatePipe stdout"); return 1; }
	if (!CreatePipe(&child_in_read, &child_in_write, &sa, 0)) { print_win_error("CreatePipe stdin"); CloseHandle(child_out_read); CloseHandle(child_out_write); return 1; }
	// Handles côté parent ne doivent pas être hérités
	SetHandleInformation(child_out_read, HANDLE_FLAG_INHERIT, 0);
	SetHandleInformation(child_in_write, HANDLE_FLAG_INHERIT, 0);
	// Prépare STARTUPINFOA
	STARTUPINFOA si{}; PROCESS_INFORMATION pi{};
	si.cb = sizeof(si);
	si.hStdOutput = child_out_write;
	si.hStdError = child_out_write;
	si.hStdInput = child_in_read;
	si.dwFlags |= STARTF_USESTDHANDLES;

	std::string cmdline_str = std::string("\"") + engine + "\"";
	std::vector<char> cmdline_buf(cmdline_str.begin(), cmdline_str.end());
	cmdline_buf.push_back('\0');

	if (!CreateProcessA(NULL, cmdline_buf.data(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
	print_win_error("CreateProcessA failed");
	CloseHandle(child_out_read); CloseHandle(child_out_write); CloseHandle(child_in_read); CloseHandle(child_in_write);
	return 1;
	}

	// Fermer les handles côté enfant dans le parent
	CloseHandle(child_in_read);
	CloseHandle(child_out_write);
	// UCI handshake
	write_cmd(child_in_write, "uci\n");
	read_available(child_out_read, 1000);
	write_cmd(child_in_write, "setoption name MultiPV value 1\nisready\n");
	std::string ready = read_available(child_out_read, 2000);
	if (ready.find("readyok") == std::string::npos) {
	// try a bit longer
	ready += read_available(child_out_read, 2000);
	if (ready.find("readyok") == std::string::npos) {
			log_err("Engine did not respond readyok");
			write_cmd(child_in_write, "quit\n");
			WaitForSingleObject(pi.hProcess, 500);
			CloseHandle(pi.hProcess); CloseHandle(pi.hThread);
			CloseHandle(child_in_write); CloseHandle(child_out_read);
			return 1;
		}
	}

	auto shutdown_engine = [&](void){
	write_cmd(child_in_write, "quit\n");
	WaitForSingleObject(pi.hProcess, 500);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(child_in_write);
	CloseHandle(child_out_read);
	};

	if (daemon_mode) {
		std::cout << "[check_moves daemon started - waiting for FENs...]" << std::endl;
		std::cout.flush();
		std::string line;
		while (std::getline(std::cin, line)) {
			if (line.empty()) continue;
			if (line == "quit" || line == "exit") break;
			// Support optional compt: parse "fen|||compt"
			std::string L = line;
			if (!L.empty() && L.back() == '\r') L.pop_back();
			std::string fen_in = L;
			int ia_compt = -1;
			auto delim = std::string("|||");
			auto p = L.find(delim);
			if (p != std::string::npos) {
				fen_in = L.substr(0, p);
				std::string comp = L.substr(p + delim.size());
				try { ia_compt = std::stoi(comp); } catch (...) { ia_compt = -1; }
			}
			std::cout << "FEN: " << fen_in << std::endl;
			std::cout.flush();
			int mv = do_perft(child_in_write, child_out_read, fen_in);
			std::cout << "Nombre de coups possibles : " << mv << std::endl;
			if (ia_compt >= 0) std::cout << "IA compt : " << ia_compt << std::endl;
			std::cout.flush();
		}
		std::cout << "[check_moves daemon stopping...]" << std::endl;
		std::cout.flush();
		shutdown_engine();
		return 0;
	}
	// single-shot: allow optional |||compt suffix in fen argument
	std::string fen_arg = fen;
	if (!fen_arg.empty() && fen_arg.back() == '\r') fen_arg.pop_back();
	int ia_compt_single = -1;
	auto delim2 = std::string("|||");
	auto p2 = fen_arg.find(delim2);
	std::string fen_only = fen_arg;
	if (p2 != std::string::npos) {
		fen_only = fen_arg.substr(0, p2);
		std::string comp = fen_arg.substr(p2 + delim2.size());
		try { ia_compt_single = std::stoi(comp); } catch (...) { ia_compt_single = -1; }
	}
	int mv = do_perft(child_in_write, child_out_read, fen_only);
	shutdown_engine();
	printf("Nombre de coups possibles : %d\n", mv);
	if (ia_compt_single >= 0) printf("IA compt : %d\n", ia_compt_single);
	return 0;
}