

struct XY {
    float x;
    float y;
    XY(){
        XY(0,0);
    }
    XY( float _x, float _y ){
        x = _x;
        y = _y;
    }
    float dot( XY other ){
        return x*other.x + y*other.y;
    }
    float operator * ( XY rhs ){
        return dot(rhs);
    }
    XY operator - ( XY rhs ){
        return XY( x-rhs.x, y-rhs.y );
    }
    XY operator + ( XY rhs ){
        return XY( x+rhs.x, y+rhs.y );
    }
    XY operator / ( float rhs ){
        return XY(x/rhs,y/rhs);
    }
};

