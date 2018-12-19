
# autocomplete
set filec
set autolist
set nobeep
set complete='enhance'


# others
setenv PATH /sw/scripts:$PATH
setenv PATH /root/work/momentum/scripts:$PATH
setenv PATH /sbin:$PATH
setenv PATH /usr/local/bin:${PATH}


# Prompt color
set prompt = "%{\033[32m%}(%n@%m %c)%{\033[0m%} "


# Command aliases
alias g grep
alias c clear
alias la ls -a
alias . cd ..
alias .. cd ../..
alias ... cd ../../..
alias .... cd ../../../..
alias ..... cd ../../../../..
alias nd 'unsetenv DISPLAY'
alias setp 'source /root/scripts/nx'
alias tools 'source /root/scripts/env_setup.csh'


# Direcory aliases
alias rp    'set prompt = "%{\033[31m%}(%n@%m %c)%{\033[0m%} "'
alias gp    'set prompt = "%{\033[32m%}(%n@%m %c)%{\033[0m%} "'
alias tkcon 'tkcon&'

alias setpp        "setenv LS_COLORS 'ow=00;34:di=00;34:fi=0:ln=00;36:pi=5:so=5:bd=5:cd=5:or=31:mi=0:ex=92:*.rpm=90'"

bindkey "^[[3~" delete-char
bindkey -k up history-search-backward
bindkey -k down history-search-forward

setenv PATH /root/scripts:${PATH}
setenv PATH /usr/local/Qt-5.0.0/bin:$PATH
setenv PATH /sw/qtcreator-2.5.2/bin:$PATH
setenv LD_LIBRARY_PATH
setenv TCLTK_INSTALL_DIR /sw/tcltk/install

setenv PATH $TCLTK_INSTALL_DIR/bin:${PATH}
setenv LD_LIBRARY_PATH $TCLTK_INSTALL_DIR/lib:${LD_LIBRARY_PATH}
setenv LD_LIBRARY_PATH $TCLTK_INSTALL_DIR/lib/tcl8.6:${LD_LIBRARY_PATH}
setenv LD_LIBRARY_PATH $TCLTK_INSTALL_DIR/lib/tk8.6:${LD_LIBRARY_PATH}
setenv TK_LIBRARY $TCLTK_INSTALL_DIR/lib/tk8.6
#
setenv PATH /sw/ws/install/bin:${PATH}
setenv LD_LIBRARY_PATH /sw/ws/install/lib:${LD_LIBRARY_PATH}
#
setenv SYSTEMC_HOME /sw/sc/install
setenv LD_LIBRARY_PATH ${SYSTEMC_HOME}/lib-linux:${LD_LIBRARY_PATH}
#
setenv PATH /sw/gtkwave/install/bin:${PATH}
setenv LD_LIBRARY_PATH /usr/local/lib:${LD_LIBRARY_PATH}
setenv PATH /sw/lisp/install/bin:${PATH}
setenv PATH /sw/trickle/install/bin:${PATH}
gp

alias g            grep -n --color=always
alias c            clear
alias la           ls -a
alias vi           vim
setpp
setenv PATH /sw/Qt5.9.0/Tools/QtCreator/bin:${PATH}
setenv PATH /sw/Qt5.9.0/5.9/gcc_64/bin:${PATH}
setenv LD_LIBRARY_PATH /sw/Qt5.9.0/5.9/gcc_64/lib:${LD_LIBRARY_PATH}
iptables --flush


setenv PATH /sw/cdn/10.20.049/bin:${PATH}
setenv PATH /sw/cdn/10.20.049/tools.lnx86/bin:${PATH}
setenv PATH /sw/cdn/license/flexlm:${PATH}
setenv PATH /sw/iverilog/bin:${PATH}
alias chdate 'date --set="1 FEB 2012 18:00:00"'
alias chmac 'source /sw/cdn/license/chmac'
alias setlic 'lmgrd -c /sw/cdn/license/license.dat'
setenv LM_LICENSE_FILE /sw/cdn/license/license.dat
setenv LD_LIBRARY_PATH /sw/systemc/lib-linux64:${LD_LIBRARY_PATH}

alias setsimv 'chdate; chmac; setlic'
setenv SYSTEMC_DISABLE_COPYRIGHT_MESSAGE 1
