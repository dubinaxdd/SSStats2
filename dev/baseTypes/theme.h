#ifndef THEME_H
#define THEME_H

#include <QObject>

namespace DowStatsStyle  {

class Theme {
    Q_GADGET
public:
    enum Value {
        Light,
        Dark,
        Pink,
        Blue
    };
    Q_ENUM(Value)
private:
    explicit Theme(){}
};
}

#endif // THEME_H
