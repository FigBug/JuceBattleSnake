class Snake
{
public:
    enum Direction
    {
        up,
        down,
        left,
        right,
    };
    
    virtual ~Snake() = default;
    
    virtual Direction getMove() = 0;
    
    String colour   = "0xffff00";
    String head     = "shades";
    String tail     = "small-rattle";
};
