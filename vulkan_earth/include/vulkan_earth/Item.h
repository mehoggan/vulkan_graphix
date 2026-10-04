#ifndef ITEM_H
#define ITEM_H

#include <cstdint>
#include <string>
#include "vulkan_earth/Tank.h"
#include "vulkan_graphix/GameCatalog.h"

class Item {
public:
    Item();
    virtual ~Item();
    /*	GETTERS AND SETTERS	*/
    virtual Item* getItemInstance() = 0;
    std::int32_t getUNIQUEIDENTIFIER();
    std::int32_t getRemaining();
    void setRemaining(std::int32_t r);
    std::string getImageFileName();
    std::string getDescription();
    std::int32_t getPrice();
    std::int32_t getPackageNum();
    std::int32_t getMaxStack();
    virtual bool causeEffectToTank(Tank* tank) = 0;
    virtual void playUseSFX();

protected:
    // Every field but the id, from the game's catalog.
    void loadSpec(const vulkan_graphix::GameCatalog::ItemSpec& spec);

    std::int32_t uniqueidentifier;
    std::string image_file_name;
    std::string description;
    std::int32_t price;
    std::int32_t package_num;
    std::int32_t max_stack;
    std::int32_t remaining;
    std::int32_t special_num;
};

#endif  //	ITEM_H