#ifndef DRAWABLE_H
#define DRAWABLE_H

class Drawable
{
public:
    Drawable();

    virtual void render_old() const = 0;
};

#endif // DRAWABLE_H
