cflags=-Wall -Wextra -g
outdir=bin

all: clean outdir main create_dict_db create_phonemes_from_txt_dataset

clean: 
	rm -rf $(outdir)

outdir:
	mkdir -p $(outdir)

main: main.c
	gcc $(cflags) -o $(outdir)/main main.c

create_dict_db: create_dict_db.c
	gcc  -o $(outdir)/create_dict_db create_dict_db.c -lsqlite3

create_phonemes_from_txt_dataset: create_phonemes_from_txt_dataset.c
	gcc  -o $(outdir)/create_phonemes_from_txt_dataset create_phonemes_from_txt_dataset.c

