#ifndef DISPLAYBASE_HPP
#define DISPLAYBASE_HPP
#include <Arduino.h>
#include <Bus.hpp>

//classe necessária para poder todos as classes derivada desta poder ter acesso aos métodos protegidos. PAra tanto é decessáriot ambem 
//declarar na seção Proteceted "friend class DisplayBase", piis desta forma evita que o usuario a cada nova classe de Driver como RA8877
//por exemplo tenha que decalrar e alterar a classe IBUs. Desta forma evita de se tocar na classe IBus sem o usuario se precoupar com 
//essa declaração ou conhecer a classe IBus.
class DisplayBase { 
  protected:
    IBus* _bus;
};


#endif //fim do include guard DISPLAYBASE_HPP