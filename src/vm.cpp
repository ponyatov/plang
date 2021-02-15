#include <vm.hpp>

int main(int argc, char *argv[]) {
    printf("\n%s\n%s\n\n",TITLE,COPYRIGHT);
    for (auto i=0;i<argc;i++) printf("argv[%i] = <%s>\n",i,argv[i]);
    printf("\n\n");
    return 0;
}
