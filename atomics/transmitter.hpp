/**
* Ben Earle and Kyle Bjornson
* ARSLab - Carleton University
*
* Blinky:
* Simple modle to toggle the LED using DEVS internal transitions.
*/

#ifndef BOOST_SIMULATION_PDEVS_transmitter_HPP
#define BOOST_SIMULATION_PDEVS_transmitter_HPP
#define TRANSFER_SIZE   4

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <stdio.h>
#include <limits>
#include <math.h>
#include <assert.h>
#include <memory>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <limits>
#include <random>
#include "../drivers/nRF24L01P.h"

using namespace cadmium;
using namespace std;

//Port definition
    struct transmitter_defs {
        struct dataOut : public out_port<double> { };
        //struct in : public in_port<bool> { };
        struct in : public in_port<double> { };
    };

    template<typename TIME>
    class transmitter {
        using defs=transmitter_defs; // putting definitions in context
        public:
            //Parameters to be overwriten when instantiating the atomic model
            TIME   slowToggleTime;
            TIME   fastToggleTime;
            // default constructor
            transmitter(PinName s, PinName t, PinName q, PinName w, PinName r, PinName a) {
              slowToggleTime  = TIME("00:00:10:00");
              fastToggleTime  = TIME("00:00:00:10");
              state.temp = new nRF24L01P(s,t,q,w,r,a);
              state.newTag = false;
              state.isCard = false;
              state.flag = 0;
              state.sending = false;
              state.counter = 0;
              state.temp->powerUp(); 
              state.commandAvailable = false;
      
            }

            // state definition
            struct state_type{
              char s[32] =  " Car 1  says hi ";
              char s2[32] = "" ;
              nRF24L01P* temp;
              bool newTag;
              bool isCard;
              int flag;
              bool sending;
              int counter;
              int recieveCounter;
              bool commandAvailable;
            };
            state_type state;
            // ports definition

            using input_ports=std::tuple<typename defs::in>;
            using output_ports=std::tuple<typename defs::dataOut>;
            int t = 0 ;



            // internal transition
            void internal_transition() {

              state.commandAvailable = false;

              if(state.sending == false || state.newTag == false)
              {


                state.temp->setTransferSize(32);
                state.temp->setReceiveMode();
                state.temp->enable();
                //for(int a = 0; a <= 2 ; a = a + 1) {
                for (int b = 0 ; b <=200; b= b+1)
                {

                   if( state.temp->readable(0)) 
                   {
                       state.temp->read(0, state.s2,sizeof(state.s2)); // reading hello world
                       printf( "Receiving data :%s  \n",state.s2);
                       printf("\n");
                       printf("\n");
                       printf("\n");
                       //state.recieveCounter--;
                       state.sending = true;
                       char *s = strstr(state.s2, "stop");      // search for string "hassasin" in buff
                       if (s != NULL)                     // if successful then s now points at "hassasin"
                       {
                           printf("Car shall stop!!!!");
                           state.commandAvailable = true;
                            break;
                       }
                       break;

                   }

              
                    else
                    {
                       //printf( "not receiving data  \n");
                    }
                  }
                    //state.temp->disable();
              }
              //if(state.counter == 0)
                //state.sending = false;

              //if(state.sending == true && state.counter > 0)
              if(state.sending == true && state.newTag == true)
              {
                  //state.counter--;
                  ///printf("newtag: %d\n", state.newTag );
                  //printf("flag:  %d\n", state.flag );  

                  //state.newTag = false;
                  //printf( "Firstrun");
                  //if( state.flag == 1 )
                  //{
                    //state.flag = 0;
                  //}
                  //else
                    state.newTag = 0;
                  printf("newTAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAG: %d\n", state.newTag );
              }
              else if(state.sending == true && state.newTag == false)
              {
                state.sending =false;
                //state.flag = false;
              }
/*
              if(state.sending == false)
              {

                state.temp->setTransferSize(32);
                state.temp->setReceiveMode();
                state.temp->enable();
                //for(int a = 0; a <= 2 ; a = a + 1) {
                for (int b = 0 ; b <=200; b= b+1)
                {

                   if( state.temp->readable(0)) 
                   {
                       state.temp->read(0, state.s2,sizeof(state.s2)); // reading hello world
                       printf( "Receiving data :%s  \n",state.s2);
                       printf("\n");
                       printf("\n");
                       printf("\n");
                       //state.recieveCounter--;
                       state.sending = true;
                       break;
                   }

              
                    else
                    {
                       printf( "not receiving data  \n");
                    }
                   }
                }*/
              






            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
/*
              printf( "nRF24L01+ Frequency    : %d MHz\r\n",  state.temp->getRfFrequency() );
              printf( "nRF24L01+ Output power : %d dBm\r\n",  state.temp->getRfOutputPower() );
              printf( "nRF24L01+ Data Rate    : %d kbps\r\n", state.temp->getAirDataRate() );
              printf( "nRF24L01+ TX Address   : 0x%010llX\r\n", state.temp->getTxAddress() );

*/
              for(const auto &x : get_messages<typename defs::in>(mbs)){
                // if the button is presssed we want to send the signal to the recevier
                printf("trans: %f\n", x );
                
                if(x == 0 )
                  break;

                state.newTag = true;
                
                if(x == 123)
                {
                  state.flag = 1;
                  state.s[0] = '1';
                  //state.counter = 2;
                  
                }
                else
                  state.s[0] = '0';

                state.sending = true;
                printf("newtag: %d\n", state.newTag );
                printf("flag:  %d\n", state.flag );  

                
              }

              

            }
            // confluence transition
            void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
              internal_transition();
             // external_transition(TIME(), std::move(mbs));
            }

            // output function
            typename make_message_bags<output_ports>::type output() const {
              typename make_message_bags<output_ports>::type bags;
              printf("newTAg in msg: %d\n", state.newTag );

              if(state.sending == true)
              {
                state.temp->setTransferSize(32);
                state.temp->setTransmitMode();
                state.temp->enable(); // enable ce pin
                printf("%i \n",sizeof(state.s));
                printf( "Sending data:%s \n",state.s);
                state.temp->write(1, const_cast<char*>(state.s),32); // writing hello world
                 printf( "Sending data:%s \n",state.s);
                printf("%i \n",sizeof(state.s));
               
                state.temp->disable();
              }
                  
              double out = 0;
              if(state.commandAvailable == true)
              {
                out = 100;
              }

              get_messages<typename defs::dataOut>(bags).push_back(out);

              return bags;
            }

            // time_advance function
            TIME time_advance() const {
              // not needed ?
              //if(state.newTag == 1)
               // 
              //if(state.flag == 1)
                //return TIME("00:00:00");;
              return slowToggleTime;

            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename transmitter<TIME>::state_type& i) {
              os << "Output: " << (i.newTag ? 1 : 0);
              return os;
            }
        };


#endif // BOOST_SIMULATION_PDEVS_BLINKY_HPP
