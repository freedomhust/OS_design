cmd_/media/neidongfu/work/VSCode/Linux_curriculum/pro3/mydev.ko := ld -r -m elf_x86_64  -z max-page-size=0x200000 -T ./scripts/module-common.lds --build-id  -o /media/neidongfu/work/VSCode/Linux_curriculum/pro3/mydev.ko /media/neidongfu/work/VSCode/Linux_curriculum/pro3/mydev.o /media/neidongfu/work/VSCode/Linux_curriculum/pro3/mydev.mod.o