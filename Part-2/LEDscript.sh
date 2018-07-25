#!/bin/bash
echo Please, enter your duty cycle pin 1 pin 2 and pin 3
echo for example 30 2 4 13
read DC P1 P2 P3
echo ${DC},${P1},${P2},${P3}

#initializing the pins
C1[0]=0 ; C1[1]=0; C1[2]=0; C2[0]=0 ; C2[1]=0; C2[2]=0; C3[0]=0 ; C3[1]=0; C3[2]=0;
CT=5
ON_TIME=$((((DC / CT)) * 1000))
OFF_TIME=$(((((100 - DC) / CT)) * 1000))

  function checkpin {
    if [ "$1" = "0" ]; then
      C[0]=11 ; C[1]=32; C[2]=0;
    elif [ "$1" = "1" ]; then
      C[0]=12 ; C[1]=28; C[2]=45;
    elif [ "$1" = "2" ]; then
      C[0]=13 ; C[1]=34; C[2]=77;
    elif [ "$1" = "3" ]; then
      C[0]=14 ; C[1]=16; C[2]=76;
    elif [ "$1" = "4" ]; then
      C[0]=6 ; C[1]=36; C[2]=0;
    elif [ "$1" = "5" ]; then
      C[0]=0 ; C[1]=18; C[2]=66;
    elif [ "$1" = "6" ]; then
      C[0]=1 ; C[1]=20; C[2]=68;
    elif [ "$1" = "7" ]; then
      C[0]=38 ; C[1]=0; C[2]=0;
    elif [ "$1" = "8" ]; then
      C[0]=40 ; C[1]=0; C[2]=0;
    elif [ "$1" = "9" ]; then
      C[0]=4 ; C[1]=22; C[2]=70;
    elif [ "$1" = "10" ]; then
      C[0]=10 ; C[1]=26; C[2]=74;
    elif [ "$1" = "11" ]; then
      C[0]=5 ; C[1]=24; C[2]=44;
    elif [ "$1" = "12" ]; then
      C[0]=15 ; C[1]=42; C[2]=0;
    elif [ "$1" = "13" ]; then
      C[0]=7 ; C[1]=30; C[2]=46;
    else
      echo wrong pin number entered
      exit
    fi
    if [ "$2" = "1" ]; then
          C1[0]=${C[0]} ; C1[1]=${C[1]}; C1[2]=${C[2]}
    elif [ "$2" = "2" ]; then
          C2[0]=${C[0]} ; C2[1]=${C[1]}; C2[2]=${C[2]}
    elif [ "$2" = "3" ]; then
          C3[0]=${C[0]} ; C3[1]=${C[1]}; C3[2]=${C[2]}
    fi
  }
  checkpin ${P1} 1;
  checkpin ${P2} 2;
  checkpin ${P3} 3;
  #echo ${C1[0]} ${C1[1]} ${C1[2]}
  #echo ${C2[0]} ${C2[1]} ${C2[2]}
  #echo ${C3[0]} ${C3[1]} ${C3[2]}


  # Exporting the pins and setting the direction
  for ((i=0; i<3; i++))
  do
    if [ "${C1[$i]}" -lt "64" ]; then
    echo -n "${C1[$i]}" > /sys/class/gpio/export
    echo -n "out" > /sys/class/gpio/gpio${C1[$i]}/direction
    fi
    if [ "${C2[$i]}" -lt "64" ]; then
    echo -n "${C2[$i]}" > /sys/class/gpio/export
    echo -n "out" > /sys/class/gpio/gpio${C2[$i]}/direction
    fi
    if [ "${C3[$i]}" -lt "64" ]; then
    echo -n "${C3[$i]}" > /sys/class/gpio/export
    echo -n "out" > /sys/class/gpio/gpio${C3[$i]}/direction
    fi
  done

  K=0
  flagx=0
  ppid=$!
mousecheck(){
$K='cat /dev/input/event2'
if [ "$K" = "1" ]; then
  flagx=1
  kill -9 $ppid 2>&1 >/dev/null
fi
}

  #Executing the sequence for blinking LED

  while true
  do
    mousecheck

    for ((j=0; j<25; j++))
    do
    echo -n "1" > /sys/class/gpio/gpio${C1[0]}/value
    usleep $ON_TIME
    echo -n "0" > /sys/class/gpio/gpio${C1[0]}/value
    usleep $OFF_TIME
    done

mousecheck
    for ((j=0; j<25; j++))
    do
    echo -n "1" > /sys/class/gpio/gpio${C2[0]}/value
    usleep $ON_TIME
    echo -n "0" > /sys/class/gpio/gpio${C2[0]}/value
    usleep $OFF_TIME
    done
mousecheck
    for ((j=0; j<25; j++))
    do
    echo -n "1" > /sys/class/gpio/gpio${C3[0]}/value
    usleep $ON_TIME
    echo -n "0" > /sys/class/gpio/gpio${C3[0]}/value
    usleep $OFF_TIME
    done
mousecheck
    for ((j=0; j<25; j++))
    do
    echo -n "1" > /sys/class/gpio/gpio${C1[0]}/value
    echo -n "1" > /sys/class/gpio/gpio${C2[0]}/value
    usleep $ON_TIME
    echo -n "0" > /sys/class/gpio/gpio${C1[0]}/value
    echo -n "0" > /sys/class/gpio/gpio${C2[0]}/value
    usleep $OFF_TIME
    done
mousecheck

    for ((j=0; j<25; j++))
    do
    echo -n "1" > /sys/class/gpio/gpio${C2[0]}/value
    echo -n "1" > /sys/class/gpio/gpio${C3[0]}/value
    usleep $ON_TIME
    echo -n "0" > /sys/class/gpio/gpio${C2[0]}/value
    echo -n "0" > /sys/class/gpio/gpio${C3[0]}/value
    usleep $OFF_TIME
    done
mousecheck
    for ((j=0; j<25; j++))
    do
    echo -n "1" > /sys/class/gpio/gpio${C3[0]}/value
    echo -n "1" > /sys/class/gpio/gpio${C1[0]}/value
    usleep $ON_TIME
    echo -n "0" > /sys/class/gpio/gpio${C3[0]}/value
    echo -n "0" > /sys/class/gpio/gpio${C1[0]}/value
    usleep $OFF_TIME
    done
mousecheck
    for ((j=0; j<25; j++))
    do
    echo -n "1" > /sys/class/gpio/gpio${C1[0]}/value
    echo -n "1" > /sys/class/gpio/gpio${C2[0]}/value
    echo -n "1" > /sys/class/gpio/gpio${C3[0]}/value
    usleep $ON_TIME
    echo -n "0" > /sys/class/gpio/gpio${C1[0]}/value
    echo -n "0" > /sys/class/gpio/gpio${C2[0]}/value
    echo -n "0" > /sys/class/gpio/gpio${C3[0]}/value
    usleep $OFF_TIME
    done
mousecheck
  done
  #Unexporting the pins
  for ((i=0; i<3; i++))
  do
    if [ "${C1[$i]}" -lt "64" ]; then
    echo -n "${C1[$i]}" > /sys/class/gpio/unexport
    fi
    if [ "${C2[$i]}" -lt "64" ]; then
    echo -n "${C2[$i]}" > /sys/class/gpio/unexport
    fi
    if [ "${C3[$i]}" -lt "64" ]; then
    echo -n "${C3[$i]}" > /sys/class/gpio/unexport
    fi
  done
