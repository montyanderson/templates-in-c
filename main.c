#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Map */

struct map_entry_s {
	char *key;
	char *value;
};

typedef struct map_s {
	size_t length;
	struct map_entry_s *array;
} Map;

Map *map_create() {
	Map *map = calloc(1, sizeof(Map));

	return map;
}

void map_set(Map *map, char *key, char *value) {
	for(size_t i = 0; i < map->length; i++) {
		if(strcmp(map->array[i].key, key) == 0) {
			free(map->array[i].value);
			map->array[i].value = strdup(value);

			return;
		}
	}

	size_t index = map->length++;
	map->array = realloc(map->array, map->length * sizeof(struct map_entry_s));

	struct map_entry_s entry = {
		.key = strdup(key),
		.value = strdup(value)
	};

	map->array[index] = entry;
}

char *map_get(Map *map, char *key) {
	for(size_t i = 0; i < map->length; i++) {
		if(strcmp(map->array[i].key, key) == 0) {
			return map->array[i].value;
		}
	}

	return NULL;
}

void map_print(Map *map) {
	for(size_t i = 0; i < map->length; i++) {
		printf("%s=%s\n", map->array[i].key, map->array[i].value);
	}

	printf("\n");
}

/* Template */

#define TEMPLATE_TEXT 1
#define TEMPLATE_VARIABLE 2

struct template_entry_s {
	char *content;
	int type;
};

typedef struct template_s {
	size_t length;
	struct template_entry_s *array;
} Template;

Template *template_create() {
	Template *template = calloc(1, sizeof(Template));

	return template;
}

void template_append(Template *template, int type, char *content) {
	struct template_entry_s entry = {
		.type = type,
		.content = strdup(content)
	};

	size_t index = template->length++;
	template->array = realloc(template->array, template->length * sizeof(struct template_entry_s));
	template->array[index] = entry;
}

Template *template_parse(char *input) {
	Template *template = template_create();

	size_t start = 0;

	for(size_t i = 0; i < 1 || input[i - 1] != 0; i++) {
		if(input[i] == '{' || input[i] == 0) {
			char *key = calloc((i - start) + 1, 1);
			memcpy(key, &input[start], i - start);

			template_append(template, TEMPLATE_TEXT, key);
			free(key);

			start = i + 1;
		}

		if(input[i] == '}') {
			char *key = calloc((i - start) + 1, 1);
			memcpy(key, &input[start], i - start);

			template_append(template, TEMPLATE_VARIABLE, key);
			free(key);

			start = i + 1;
		}
	}

	return template;
}

char *template_render(Template *template, Map *map) {
	char *output = NULL;
	size_t output_length = 0;

	for(size_t i = 0; i < template->length; i++) {
		struct template_entry_s entry = template->array[i];

		size_t index = output_length;

		if(entry.type == TEMPLATE_TEXT) {
			size_t length = strlen(entry.content);

			output_length += length;
			output = realloc(output, output_length);

			memcpy(&output[index], entry.content, length);
		}

		if(entry.type == TEMPLATE_VARIABLE) {
			char *value = map_get(map, entry.content);

			size_t length = strlen(value);

			output_length += length;
			output = realloc(output, output_length);

			memcpy(&output[index], value, length);
		}
	}

	return output;
}

void template_print(Template *template) {
	printf("printing %lu\n", template->length);

	for(size_t i = 0; i < template->length; i++) {
		printf("'%s' %d\n", template->array[i].content, template->array[i].type);
	}
}

int main() {
	Template *template = template_parse("Hello, {name}!");
	//template_print(template);

	Map *map = map_create();

	map_set(map, "name", "monty");

	//map_print(map);
	char *output = template_render(template, map);

	printf("%s\n", output);
}
