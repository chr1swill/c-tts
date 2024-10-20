create_dict_db: create_dict_db.c
	gcc -Wall -Wextra -g -o dest/create_dict_db create_dict_db.c -lsqlite3
