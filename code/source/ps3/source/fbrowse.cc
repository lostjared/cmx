#include"cmx_browser.h"
#include<dirent.h>


namespace cmx {
	namespace debug {

		bool FileBrowser::isDir(std::string fname) {
			DIR *d = opendir(fname.c_str());
			if(d != 0) {
				closedir(d);
				return true;
			}
			return false;
		}

		void FileBrowser::fillVector(std::vector<std::string> &v) {
			DIR *dir = opendir(dir_path.c_str());
			if(dir == 0) {   return; }
			dirent *e;

			while ((e = readdir(dir)) != NULL) {
				v.push_back(e->d_name);
			}
			closedir(dir);
			dir_offset = 0;
		}
	}
}
