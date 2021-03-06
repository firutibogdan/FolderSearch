#include "stdafx.h"

using namespace std;

vector<wstring> get_files(_TCHAR* path, _TCHAR* file_mask) {
	vector<wstring> files;
	set<wstring> unique_extensions;
	_TCHAR* token;
	_TCHAR* buffer;

	// break the file_mask in tokens in order to get files with given extensions
	token = _wcsdup(file_mask);
	token = wcstok_s(token, TEXT(";"), &buffer);
	while (token != nullptr && token != 0) {

		// check if there are duplicates in extensions
		if (unique_extensions.find(token) == unique_extensions.end()) {
			_TCHAR search_path[2048] = { 0 };
			WIN32_FIND_DATAW FindData;
			HANDLE FileHandle = INVALID_HANDLE_VALUE;
			unsigned long token_length = lstrlen(token);

			// insert in set in order to keep track of used extensions
			unique_extensions.insert(token);
			wsprintf(search_path, L"%s\\*", path);
			FileHandle = FindFirstFile(search_path, &FindData);

			// if error occured, exit
			if (FileHandle == INVALID_HANDLE_VALUE) {
				cerr << "Error occured!\n";
				exit(-1);
			} else {
				do {
					// check if it's a directory
					if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						// I have to go inside it and search for files
						_TCHAR new_path[2048];
						wsprintf(new_path, L"%s\\%s", path, FindData.cFileName);
						// eliminate . (current directory) and .. (parent of this directory)
						if (wcscmp(L".", FindData.cFileName) != 0 && wcscmp(L"..", FindData.cFileName) != 0) {
							// get recursively files inside folder and add to the vector
							vector<wstring> folder = get_files(new_path, token);
							files.insert(files.end(), folder.begin(), folder.end());
						}
					} else {
						// or a simple file
						wstring name = FindData.cFileName;
						// check if I have to search for an extension, and find files with it
						// differentiate 2 extensions with same endings (ex. ".txt" ".ttxt")
						if (wcscmp(token, L"*") != 0 && name.find('.') != string::npos && name.length() > token_length) {
							if (name.compare(name.length() - token_length, token_length, token) == 0 && 
								name[name.length() - token_length - 1] == '.') {
								name.insert(name.begin(), '\\');
								files.push_back(path + name);
							}
						} else if (wcscmp(token, L"*") == 0) {
							// or simply get all files
							name.insert(name.begin(), '\\');
							files.push_back(path + name);
						}
					}
				} while (FindNextFile(FileHandle, &FindData));
				CloseHandle(FileHandle);
			}
		}
		// get to the next extension
		token = wcstok_s(nullptr, TEXT(";"), &buffer);
	}
	return files;
}

void search_phrase(_TCHAR* phrase, _TCHAR* output_file, vector<wstring> files) {
	wofstream fout(output_file);
	for (wstring file : files) {
		wifstream fin(file);
		int line_counter = 0;
		wstring buffer;
		while (getline(fin, buffer)) {
			line_counter++;
			if (buffer.find(phrase) != wstring::npos) {
				fout << file << "(" << line_counter << "): " << buffer << "\n";
			}
		}
		fin.close();
	}
	fout.close();
}

int _tmain(int argc, _TCHAR* argv[]) {

	if (argc != 5) {
		cerr << "Incorrect number of parameters!\n";
		cerr << "Usage: FolderSearch directory_path file_mask search_phrase output_file\n";
		return -1;
	}

	vector<wstring> files = get_files(argv[1], argv[2]);
	search_phrase(argv[3], argv[4], files);

	return 0;
}