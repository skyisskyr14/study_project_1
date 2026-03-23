#include <stdio.h>
#include "ui.h"
#include "common.h"

int main(void) {
    printf("%s v%s\n", APP_NAME, APP_VERSION);
    printf("说明：默认数据库配置在 src/ui.c 中，可按需修改。\n");
    run_app();
    return 0;
}
