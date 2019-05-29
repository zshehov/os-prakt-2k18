#include <unistd.h>
#include <err.h>

int main() {
   int pid = fork();

   if (pid == 0) {
      if (execl("/bin/sleep", "sleepChild", "5", (char*) NULL) == -1) {
         errx(5, "whops");
      }
   } else {
      // в родителя спим с 5 секунди отгоре, за да може детето да умре преди нас
      execl("/bin/sleep", "sleepParent", "10", (char*) NULL);
   }

   // $ ps -e -o pid,ppid,comm,stat | grep sleep
   // 29684  2085 sleep           S+
   // 29685 29684 sleep           S+
   //
   // след като са минали 5те секунди в детето
   // 
   // $ ps -e -o pid,ppid,comm,stat | grep sleep
   // 29684  2085 sleep           S+
   // 29685 29684 sleep <defunct> Z+   <-- станахме свидетели на зомби
   //
   // детето умря, ОС изчисти нещата за него, но остави някаква информация за това
   // как е завършило, за да може евентуално родителят ако се интересува от тази
   // информация, да я вземе (резултатът е мъртъв процес, който не е съвсем умрял = зомби)
}
