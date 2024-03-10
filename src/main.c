#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>

int main(int argc, char *argv[]) {
    if(argc != 3) {
        fprintf(stderr, "please provide the target executable path aswell as the output file path\n");
        return EXIT_FAILURE;
    }
    const char *executable_path = argv[1];
    const char *output_path = argv[2];

    FILE *elf_file = fopen(executable_path, "r+b");
    if(!elf_file) {
        perror("Error opening executable");
        return EXIT_FAILURE;
    }

    Elf64_Ehdr elf_header;
    fread(&elf_header, sizeof(Elf64_Ehdr), 1, elf_file);

    Elf64_Shdr section_headers[elf_header.e_shnum];
    fseek(elf_file, elf_header.e_shoff, SEEK_SET);
    fread(section_headers, sizeof(Elf64_Shdr), elf_header.e_shnum, elf_file);

    Elf64_Shdr *shstrtab = &section_headers[elf_header.e_shstrndx];
    char *section_names = malloc(shstrtab->sh_size);
    fseek(elf_file, shstrtab->sh_offset, SEEK_SET);
    fread(section_names, 1, shstrtab->sh_size, elf_file);

    Elf64_Shdr *text = NULL;
    for(int i = 0; i < elf_header.e_shnum; i++) {
        if(section_headers[i].sh_type == SHT_PROGBITS &&
                section_headers[i].sh_flags & SHF_EXECINSTR) {
            if(strcmp(section_names + section_headers[i].sh_name, ".text") == 0) {
                text = &section_headers[i];
                break;
            }
        }
    }

    if(!text) {
        fprintf(stderr, "couldnt locate text section\n");
        return EXIT_FAILURE;
    }

    Elf64_Shdr *symbol_table = NULL;
    for(int i = 0; i < elf_header.e_shnum; i++) {
        if(section_headers[i].sh_type == SHT_SYMTAB) {
            symbol_table = &section_headers[i];
            break;
        }
    }

    if(!symbol_table) {
        fprintf(stderr, "couldnt locate symbol table\n");
        return EXIT_FAILURE;
    }

    Elf64_Sym *symbols = malloc(symbol_table->sh_size);
    fseek(elf_file, symbol_table->sh_offset, SEEK_SET);
    fread(symbols, 1, symbol_table->sh_size, elf_file);
    int symbol_amount = symbol_table->sh_size / sizeof(Elf64_Sym);

    for(int i = 0; i < symbol_amount; i++) {
        if(ELF64_ST_TYPE(symbols[i].st_info) == STT_FUNC) {
            printf("%d\n", symbols[i].st_name);
        }
    }

    fclose(elf_file);
    return EXIT_SUCCESS;
}
