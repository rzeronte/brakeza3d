#ifndef BRAKEDA3D_BRAKEZA3D_H
#define BRAKEDA3D_BRAKEZA3D_H


class Brakeza3D {
public:
    Brakeza3D();
    ~Brakeza3D();

    static Brakeza3D* getInstance();
    static Brakeza3D* instance;
};


#endif //BRAKEDA3D_BRAKEZA3D_H
