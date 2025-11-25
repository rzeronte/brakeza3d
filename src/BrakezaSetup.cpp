

#include "../include/BrakezaSetup.h"

BrakezaSetup *BrakezaSetup::instance = nullptr;

BrakezaSetup *BrakezaSetup::get()
{
    if (instance == nullptr) {
        instance = new BrakezaSetup();
    }

    return instance;
}
