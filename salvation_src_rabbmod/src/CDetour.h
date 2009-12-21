// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org
// (ported to unix by kobj and blof)

// GPL License - see http://opensource.org/licenses/gpl-license.php
// Copyright 2006 *nixCoders team - don't forget to credit us

#include <fcntl.h>
#include <gelf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

class CDetour
{
   public:
      CDetour();
      ~CDetour();

      void *CreateDetour(void *orig, void *dest);
      void RemoveDetour(void *orig, void *tramp);
	  bool GOTHook(const char *symbol, void *hook);
	  int IsStripped(char *filename);
	  GElf_Shdr *getSectionHeader(const char *sectionName);
	  GElf_Shdr *getSectionHeader(const char *filename, const char *sectionName);

   private:
      int disassemble_x86(void *addr);
	  Elf *initElf(const char *filename);
	  Elf_Scn *getSection(Elf *elf, const char *sectionName);
	  GElf_Sym *getSymbol(char *filename, char *symbol);
};
