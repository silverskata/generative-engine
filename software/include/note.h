#define WHOLENOTE 32
#define SEMINOTE 16
#define DOTTEDSEMI 24
#define QUARTERNOTE 8
#define DOTTEDQUARTER 12
#define EIGHTNOTE 4
#define DOTTEDEIGHT 6
#define SIXTEENTHNOTE 2
#define DOTTEDSIXTEENTH 3
#define THIRTYSECONDNOTE 1

/**
 *	@brief holds note information
 */
typedef struct
{
    uint8_t legato; // length of the gate, same length as length gives full legato
    uint8_t type;   // For placing and removing notes
    int16_t value;  // the 12 semitones
    uint8_t octave;
    uint8_t length;
    int8_t modulation;
    bool protected;
} note;