#include "boost/asio/serial_port.hpp"
#include <boost/asio.hpp> 
#include <ncurses.h>
#include <string>


using namespace boost;
using std::string;

struct Arrow_Locations{
  //up[0] = row, up[1] = col start, up[2] = length
  short up[3];
  short down[3];
  short left[3];
  short right[3];

  Arrow_Locations(short start_row, short up_col){
    up[0] = start_row;
    up[1] = up_col;
    up[2] = 2;

    down[0] = start_row + 4; down[1] = up_col - 1;
    down[2] = 4;

    left[0] = start_row + 2;
    left[1] = up_col - 5;
    left[2] = 4;

    right[0] = start_row + 2;
    right[1] = up_col + 5;
    right[2] = 5;
  }
};

Arrow_Locations draw_arrows(short start_row, short up_col){
  mvprintw(start_row, up_col, "UP");
  mvprintw(start_row + 4, up_col - 1, "DOWN");
  mvprintw(start_row + 2, up_col - 5, "LEFT");
  mvprintw(start_row + 2, up_col + 5, "RIGHT");

  return Arrow_Locations(start_row, up_col);
}



class Motors{

  unsigned char x_pos, y_pos, x_motor_cmd, y_motor_cmd;
  asio::io_service io;
  asio::serial_port * port;
  unsigned char c[2];

public:

  ~Motors(){
    port->close();
    delete port;
  }
  Motors(){
    x_pos = 90;
    y_pos = 40;

    y_motor_cmd = 0;
    x_motor_cmd = 1;

   port = new asio::serial_port(io);

  }

  void move_up(int amt){
    y_pos += amt;
    char commands[2];
    commands[0]=0; 
    commands[1]=y_pos;
    port->write_some(boost::asio::buffer(commands, 2));
  }

  void move_down(int amt){
    y_pos -= amt;
    char commands[2];
    commands[0]=0; 
    commands[1]=y_pos;
    port->write_some(boost::asio::buffer(commands, 2));
  }

  void move_left(int amt){
    x_pos -= amt;
    char commands[2];
    commands[0]=1; 
    commands[1]=x_pos;
    port->write_some(boost::asio::buffer(commands, 2));
  }

  void move_right(int amt){
    x_pos += amt;
    char commands[2];
    commands[0]=1; 
    commands[1]=x_pos;
    port->write_some(boost::asio::buffer(commands, 2));

  }

  bool init_serial(int argc, char **argv){
    //intialize serial
    string port_name(argv[1]);


    // Argument 2 is the baudrate
    unsigned long baud = 0;
#if defined(WIN32) && !defined(__MINGW32__)
    sscanf_s(argv[2], "%lu", &baud);
#else
    sscanf(argv[2], "%lu", &baud);
#endif


    port->open(port_name);
    port->set_option(asio::serial_port_base::baud_rate(baud));
    // port, baudrate, timeout in milliseconds

    return port->is_open();

  }

  void init_motors(int x, int y){

    char commands[2];
    x_pos = x;
    y_pos = y;
   

    commands[0]=1; 
    commands[1]=x_pos;
    port->write_some(boost::asio::buffer(commands, 2));
    commands[0]=0; 
    commands[1]=y_pos;
    port->write_some(boost::asio::buffer(commands, 2));
  }

  void move_motors(Arrow_Locations arrows, int ch){
    short r, c, l;
    unsigned char commands[2];
    mvprintw(0,0,"ch:%d", ch);
    mvprintw(1,0,"ch=key_right:%d", ch==KEY_RIGHT);
    //mvprintw(2,0,"val: %u, %u", c[0],c[1]);

    //Reset the bolding
    c=0;
    l=-1;

    mvchgat(arrows.right[0], c, l, A_NORMAL, 0, NULL);
    mvchgat(arrows.left[0] , c, l, A_NORMAL, 0, NULL);
    mvchgat(arrows.up[0]   , c, l, A_NORMAL, 0, NULL);
    mvchgat(arrows.down[0] , c, l, A_NORMAL, 0, NULL);

    switch(ch){
    case 'd':
      x_pos += 3;
      refresh();
      r = arrows.right[0];
      c = arrows.right[1];
      l = arrows.right[2];
      refresh();
      mvchgat(r, c, l, A_BOLD, 0, NULL);
      commands[0]=1; 
      commands[1]=x_pos;
      port->write_some(boost::asio::buffer(commands, 2));

      break;
    case 'a':
      x_pos -= 3;
      refresh();
      r = arrows.left[0];
      c = arrows.left[1];
      l = arrows.left[2];
      mvchgat(r, c, l, A_BOLD, 0, NULL);
      commands[0]=1; 
      commands[1]=x_pos;
      port->write_some(boost::asio::buffer(commands, 2));
      break;
    case 'w':
      y_pos += 3;
      refresh();
      r = arrows.up[0];
      c = arrows.up[1];
      l = arrows.up[2];
      mvchgat(r, c, l, A_BOLD, 0, NULL);
      commands[0]=0; 
      commands[1]=y_pos;
      port->write_some(boost::asio::buffer(commands, 2));
      break;
    case 's':
      y_pos -= 3;
      refresh();
      r = arrows.down[0];
      c = arrows.down[1];
      l = arrows.down[2];
      mvchgat(r, c, l, A_BOLD, 0, NULL);
      commands[0]=0; 
      commands[1]=y_pos;
      port->write_some(boost::asio::buffer(commands, 2));

      break;
    }
  }
};

