#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

void error (const char* message, int status)
{
	if (message)
		fputs(message, stderr);
	exit(status);
}

#define TEXT_BUF_SIZE 2048
char text[TEXT_BUF_SIZE];
int text_size = 0;
int has_any_text = 0;

void push_back_text (char c)
{
	if (text_size >= TEXT_BUF_SIZE-1)
		error("Too much text in tag\n", 1);
	text[text_size++] = c;
	text[text_size] = '\0';
	has_any_text |= !isspace(c);
}

void clear_text (void)
{
	text_size = 0;
	text[0] = '\0';
	has_any_text = 0;
}


#define TAG_BUF_SIZE 256
char tag[TAG_BUF_SIZE];
int tag_size;

void push_back_tag (char c)
{
	if (tag_size >= TAG_BUF_SIZE-1)
		error("Tag too long\n", 2);
	tag[tag_size++] = c;
	tag[tag_size] = '\0';
}

void clear_tag (void)
{
	tag_size = 0;
	tag[0] = '\0';
}

int main (void)
{
	int c;
	enum state_t {
		SEEK_TAG,
		IN_TAG_START,
		IN_OPENING_TAG,
		IN_CLOSING_TAG
	} state = SEEK_TAG;
	int leading_newline = 0;

	int indent_depth = 0;
#define INDENT_STRING "  "

	while ((c = fgetc(stdin)) != EOF) {

		switch (state) {
		case SEEK_TAG:
			if (c == '<')
				state = IN_TAG_START;
			else
				push_back_text(c);

			break;
		case IN_TAG_START:

			if (isspace(c))
				continue;

			if (c == '/') {
				state = IN_CLOSING_TAG;
			} else {
				clear_tag();
				push_back_tag(c);
				state = IN_OPENING_TAG;
			}

			break;
		case IN_OPENING_TAG:

			if (has_any_text) {
				error("Cannot represent both text "
				      "and subtags in a tag", 3);
			}
			if (isspace(c))
				continue;

			if (c == '>') {
				if (leading_newline)
					fputc('\n', stdout);
				leading_newline = 1;
				for (int i = 0; i < indent_depth; i++)
					fputs(INDENT_STRING, stdout);
				fputs(tag, stdout);
				fputc(':', stdout);

				indent_depth++;
				state = SEEK_TAG;
				clear_text();
			} else {
				push_back_tag(c);
			}

			break;
		case IN_CLOSING_TAG:

			if (c == '>') {
				indent_depth--;
				state = SEEK_TAG;
				if (has_any_text) {
					fputs(" \"", stdout);
					fputs(text, stdout);
					fputc('\"', stdout);
				}
				clear_text();
			}

			break;
		}

	}

	if (leading_newline)
		fputc('\n', stdout);

	return 0;
}
