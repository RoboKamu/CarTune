void t5omsi(void);                  //Initialize T5 to a one (1) ms counter.
int t5expq(void);                   //Returns T if T5 has expired else F.

void colinit(void);                 //Initialize the column hw driver.
int colset(void);                   //Returns, and activates, "next" column.

void l88init(void);                 //Initialize the LED8*8 matrix hw driver.
void l88row(int row);               //Look-up, and transfer, correct row data.
void l88mem(int row, int data);     //Update LED8*8 <row> info with <data>.

void keyinit(void);                 //Initialzie the Keyboard hw driver.
int keyscan(void);                  //Scan the keybord, and return any key data.