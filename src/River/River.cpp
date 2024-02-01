
#include "River.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

River::River() {
	char	path[256];

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configParam(PARAM_SELECT_WHEEL, 0, 1, 0, "Select file wheel", "%", 0, 100);
	configSwitch(PARAM_SELECT_CLICK, 0, 1, 0, "Select file click");

	this->buffer = NULL;
	this->buffer_init();

	getcwd(path, sizeof(path));
	this->dir_path = path;
	this->dir_path += "/";
	this->dir_read();
}

River::~River() {
	if (this->buffer)
		free(this->buffer);
}

void River::process(const ProcessArgs& args) {
	int		length;

	if (this->buffer == NULL)
		return;

	/// [1] HANDLE PLAY

	/// [2] HANDLE SELECTOR

	if (args.frame % 32 != 0)
		return;

	/// SELECT WHEEL
	length = this->dir.size();
	this->dir_selected = length * this->params[PARAM_SELECT_WHEEL].getValue();
	if (this->dir_selected > length - 1)
		this->dir_selected = length - 1;

	/// SELECT CLICK
	if (this->button_select.process(
	this->params[PARAM_SELECT_CLICK].getValue())) {
		this->dir_open();
	}
}

void River::buffer_init(void) {
	size_t	length;
	float	rate;

	rate = APP->engine->getSampleRate();
	length = 10.0 * rate;
	if (this->buffer)
		free(this->buffer);
	this->buffer = (float*)malloc(sizeof(float) * length);
	this->buffer_length = length;
	this->buffer_i = 0;
	this->buffer_rate = rate;
}

void River::dir_read(void) {
	DIR				*dir;
	struct dirent	*file;
	struct stat		file_stat;
	std::string		path;

	this->dir.clear();
	this->dir.emplace_back((char*)"..", RIVER_DIR);
	dir = opendir(this->dir_path.c_str());
	if (dir) {
		while ((file = readdir(dir)) != NULL) {
			/// SKIP . && .. DIRECTORY
			if (file->d_name[0] == '.') {
				if (file->d_name[1] == 0)
					continue;
				else if (file->d_name[1] == '.' && file->d_name[2] == 0)
					continue;
			}
			/// INSERT DIRECTORY
			path = this->dir_path + file->d_name;
			stat(path.c_str(), &file_stat);
			//if (file_stat.st_mode & S_IFDIR)
			if ((file_stat.st_mode & S_IFMT) == S_IFDIR)
				this->dir.emplace_back(file->d_name, RIVER_DIR);
			else
				this->dir.emplace_back(file->d_name, RIVER_FILE);
		}
		closedir(dir);
	}
}

void River::dir_open(void) {
	RiverFile	*file;
	std::string	path;
	size_t		index;

	file = &(this->dir[this->dir_selected]);
	if (file->type == RIVER_DIR) {
		/// GO TO PARENT
		if (file->name[0] == '.'
		&& file->name[1] == '.'
		&& file->name[2] == '/'
		&& file->name[3] == 0) {
			index = this->dir_path.find_last_of("/\\",
			/**/ this->dir_path.size() - 2);
			if (index != this->dir_path.npos)
				this->dir_path = this->dir_path.substr(0, index + 1);
		/// GO TO DIRECTORY
		} else {
			this->dir_path = this->dir_path + file->name;
		}
		this->dir_read();
	}
}

Model* modelRiver = createModel<River, RiverWidget>("Biset-River");
