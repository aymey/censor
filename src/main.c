#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>

int main(int argc, char *argv[]) {
    if(argc != 2) {
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
    fread(&elf_header, 1, sizeof(Elf64_Ehdr), elf_file);

    Elf64_Shdr section_headers[elf_header.e_phnum];
    fseek(elf_file, elf_header.e_shoff + elf_header.e_shstrndx * sizeof(Elf64_Shdr), SEEK_SET);
    fread(&section_headers, sizeof(Elf64_Shdr), elf_header.e_phnum, elf_file);

    Elf64_Shdr strtab_header = section_headers[elf_header.e_shstrndx];
    char *section_names = malloc(strtab_header.sh_size);
    fseek(elf_file, strtab_header.sh_offset, SEEK_SET);
    fread(section_names, 1, strtab_header.sh_size, elf_file);

    // Find the .text section
    Elf64_Shdr text_section_header;
    for (int i = 0; i < elf_header.e_shnum; i++) {
        if (section_headers[i].sh_type == SHT_PROGBITS &&
            strcmp(section_names + section_headers[i].sh_name, ".text") == 0) {
            text_section_header = section_headers[i];
            break;
        }
    }

    if(text_section_header.sh_size == 0) {
        fprintf(stderr, "couldnt locate text section\n");
        return EXIT_FAILURE;
    }

    // Find symbol table
    Elf64_Shdr symtab_section_header;
    for (int i = 0; i < elf_header.e_shnum; i++) {
        if (section_headers[i].sh_type == SHT_SYMTAB) {
            symtab_section_header = section_headers[i];
            break;
        }
    }

    if(symtab_section_header.sh_size == 0) {
        fprintf(stderr, "couldnt locate symbol table\n");
        return EXIT_FAILURE;
    }

    Elf64_Sym *symbols = malloc(symtab_section_header.sh_size);
    fseek(elf_file, symtab_section_header.sh_offset, SEEK_SET);
    fread(symbols, 1, symtab_section_header.sh_size, elf_file);
    int symbol_count = symtab_section_header.sh_size / sizeof(Elf64_Sym);

    // Iterate through symbols to find function entry points
    for (int i = 0; i < symbol_count; i++) {
        if (ELF64_ST_TYPE(symbols[i].st_info) == STT_FUNC) {
            // Get the address of the function
            Elf64_Addr func_address = symbols[i].st_value;
            // Calculate the end address of the function
            Elf64_Addr end_address = func_address + symbols[i].st_size;
            // Move file pointer to the end of the function
            fseek(elf_file, text_section_header.sh_offset + end_address, SEEK_SET);
            // Write your bytes here
            unsigned char bytes_to_write[] = {0x90, 0x90}; // Example bytes (NOP instructions)
            fwrite(bytes_to_write, sizeof(bytes_to_write), 1, elf_file);
        }
    }

    fclose(elf_file);
    free(section_names);
    free(symbols);
    return EXIT_SUCCESS;
}
