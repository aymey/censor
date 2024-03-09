#include <stdlib.h>
#include <stdio.h>
#include <elf.h>

int main(int argc, char *argv[]) {
    if(argc != 2) {
        fprintf(stderr, "please provide the target executable path aswell as the output file path\n");
        return EXIT_FAILURE;
    }

    const char *executable_path = argv[0];
    const char *output_path = argv[1];

    FILE *elf_file = fopen(executable_path, "r+b");
    if(!elf_file) {
        perror("Error opening executable");
        return EXIT_FAILURE;
    }

    Elf64_Ehdr elf_header;
    fread(&elf_header, 1, sizeof(Elf64_Ehdr), elf_file);

    Elf64_Shdr text_section;
    fseek(elf_file, elf_header.e_shoff + elf_header.e_shstrndx * sizeof(Elf64_Shdr), SEEK_SET);
    fread(&text_section, 1, sizeof(Elf64_Shdr), elf_file);

    fclose(elf_file);

    printf("%d", text_section.sh_name);

    return EXIT_SUCCESS;
}
