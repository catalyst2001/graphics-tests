#include "rstring_utility.h"

char *trim_string(char *p_string)
{
	if (!p_string[0])
		return p_string;

	int i, skipspaces;
	bool is_last_space = false;
	for (i = 0, skipspaces = 0; p_string[i]; i++) {
		if (p_string[i] == ' ') {
			skipspaces++;
			is_last_space = true;
		}
		else if (is_last_space) {
			int len = strlen(p_string);
		}
	}

	return p_string;
}

bool is_line_comment(const char *p_string, int commentsym)
{
	for (int i = 0; p_string[i]; i++) {
		if (p_string[i] == ' ')
			continue;

		if (p_string[i] == commentsym)
			return true;
	}
	return false;
}
