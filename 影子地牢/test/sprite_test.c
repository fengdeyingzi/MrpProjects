#include "src/spritex.h"

SpriteX *spx;
int main(){
    printf("test1............ %d", sizeof(int32_t));
    
    spx = createSpriteXWithImageName("a.sprite", "a.png");
    setVisible(spx, 1);
    update(spx, 100);
    return 0;
}

