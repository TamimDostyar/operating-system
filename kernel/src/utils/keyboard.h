enum SCAN {
    // Escape
    scanESC = 0x01,

    // Number row
    scanBackQuote = 0x29,   // ` ~
    scan1 = 0x02,
    scan2 = 0x03,
    scan3 = 0x04,
    scan4 = 0x05,
    scan5 = 0x06,
    scan6 = 0x07,
    scan7 = 0x08,
    scan8 = 0x09,
    scan9 = 0x0A,
    scan0 = 0x0B,
    scanMinus = 0x0C,       // - _
    scanEqual = 0x0D,       // = +
    scanBackspace = 0x0E,

    // Tab and top letter row
    scanTab = 0x0F,
    scanQ = 0x10,
    scanW = 0x11,
    scanE = 0x12,
    scanR = 0x13,
    scanT = 0x14,
    scanY = 0x15,
    scanU = 0x16,
    scanI = 0x17,
    scanO = 0x18,
    scanP = 0x19,
    scanLeftBracket = 0x1A,  // [ {
    scanRightBracket = 0x1B, // ] }
    scanEnter = 0x1C,

    // Home row
    scanCapsLock = 0x3A,
    scanA = 0x1E,
    scanS = 0x1F,
    scanD = 0x20,
    scanF = 0x21,
    scanG = 0x22,
    scanH = 0x23,
    scanJ = 0x24,
    scanK = 0x25,
    scanL = 0x26,
    scanSemicolon = 0x27,   // ; :
    scanQuote = 0x28,       // ' "

    // Bottom row
    scanLeftShift = 0x2A,
    scanZ = 0x2C,
    scanX = 0x2D,
    scanC = 0x2E,
    scanV = 0x2F,
    scanB = 0x30,
    scanN = 0x31,
    scanM = 0x32,
    scanComma = 0x33,       // , <
    scanDot = 0x34,         // . >
    scanSlash = 0x35,       // / ?
    scanRightShift = 0x36,

    // Modifier keys
    scanLeftCtrl = 0x1D,
    scanLeftAlt = 0x38,
    scanSpace = 0x39,

    scanRightAlt = 0xE038,  // extended, handle separately
    scanRightCtrl = 0xE01D, // extended

    // Function keys
    scanF1 = 0x3B,
    scanF2 = 0x3C,
    scanF3 = 0x3D,
    scanF4 = 0x3E,
    scanF5 = 0x3F,
    scanF6 = 0x40,
    scanF7 = 0x41,
    scanF8 = 0x42,
    scanF9 = 0x43,
    scanF10 = 0x44,
    scanF11 = 0x57,
    scanF12 = 0x58,

    // Editing keys
    scanDelete = 0x53,
    scanInsert = 0x52,
    scanHome = 0x47,
    scanEnd = 0x4F,
    scanPageUp = 0x49,
    scanPageDown = 0x51,

    // Arrow keys 
    scanUpArrow = 0x48,
    scanDownArrow = 0x50,
    scanLeftArrow = 0x4B,
    scanRightArrow = 0x4D,
};

static inline int isUpper(char c){ return c >= 'A' && c <= 'Z'; }
static inline int isLower(char c){ return c >= 'a' && c <= 'z'; }
static inline int isNumber(char c){ return c >= '0' && c <= '9'; }

void keyboard_init(void);
void keyboardHandler(void);
char keyboard_getchar(void);
