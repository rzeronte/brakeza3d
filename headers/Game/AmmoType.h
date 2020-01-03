//
// Created by darkhead on 3/1/20.
//

#ifndef BRAKEDA3D_AMMOTYPE_H
#define BRAKEDA3D_AMMOTYPE_H


#include <string>

class AmmoType {
public:
    AmmoType();

    const std::string &getName() const;
    void  setName(const std::string &name);

    const std::string &getClassname() const;
    void  setClassname(const std::string &classname);

    int  getAmount() const;
    void setAmount(int ammo);

private:
    std::string name;
    std::string classname;
    int amount;
public:
};


#endif //BRAKEDA3D_AMMOTYPE_H
