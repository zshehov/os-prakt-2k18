#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>

int main() {

   // нека да запишем нещо във файл
   char chars[] = "This is some text that will go into the file";

   // не записваме текста в char*, понеже така щяхме да загубим възможността да
   // изброим байтовете със sizeof, тъй като
   //
   // sizeof( <нещо от тип масив> ) == колко байта заема масива
   // sizeof( <нещо от тип пойнтър> ) == колко байта заема пойнтъра = 4 или 8
   //
   // NOTE: int arr_with_ints[10]; -> sizeof(arr_with_ints) == 40 (т.е. колко байта е масивът, а НЕ колко елемента)
   // sizeof(arr_with_ints) / sizeof(int) == 10
   
   // NOTE: Ако все пак имахме char*, можеше да вземем дължината му със
   // strlen / strnlen, като strlen("asd") == 3, т.е. не броим null terminator-а, (btw, strlen е линейна операция, тъй като брои char по char докато не стигне null terminator)

   int fd = open("test", O_CREAT | O_RDWR, 00700);
   if (fd == -1) {
      errx(3, "Couldn't open file");
   }
   
   int i;

   // можем да пишем байт по байт във файла:
   for (i = 0; i < sizeof(chars); ++i) {
      // &char[i] == адресът на i-тият char в масива
      write(fd, &chars[i], 1);
   }

   // $ cat test
   // This is some text that will go into the file
   
   // обаче като пишем байт по байт, викаме по един write за всеки байт
   // write е system call, а те са скъпи - https://www.quora.com/Why-are-system-calls-expensive-in-operating-systems
   
   // в случая можем просто да запишем целият масив наведнъж и ще направим само един
   // system call write
   
   // връщаме пойтъра в началото и ще мажем върху вече написаното, за да не си
   // играем с нов файл
   lseek(fd, 0, SEEK_SET);

   write(fd, &chars, sizeof(chars));
   // $ cat test
   // This is some text that will go into the file

   lseek(fd, 0, SEEK_SET);
   // същото важи и за system call-а read, за това има практика да се чете в буфери
   // вместо байт по байт както:
   
   char c;
   int t_count = 0;
   // да намерим бройката на 't'
   while (read(fd, &c, 1) > 0) {
      if (c == 't') { 
         ++t_count;
      }
   }
   printf("ts are: %d\n", t_count);
   // ts are: 6
   
   lseek(fd, 0, SEEK_SET);

   // сега да използваме буфер
   char buffer[4096];
   int read_size = 0;
   t_count = 0;

   // всеки път казваме на read да се опита да прочете парче от 4096 байта наведнъж,
   // апък read-ът ще ни върне колко е успял да прочете всъщност
   while ( (read_size = read(fd, &buffer, sizeof(buffer))) > 0 ) {
      for (i = 0; i < read_size; ++i) {
         // в момента буферът ни е в РАМ-та, където достъпването на елемент от
         // масива е в порядък по-бързо от правенето на system call
         if (buffer[i] == 't') {
            ++t_count;
         }
      }
   }

   printf("ts are: %d\n", t_count);
   // ts are: 6

   close(fd);
}
   
