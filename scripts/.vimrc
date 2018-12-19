syntax on
set cin
set hls
set smartindent
set expandtab
%retab
set tabstop=4
set shiftwidth=4
set guifont=Fantasque\ Sans\ Mono\ 13
set guioptions=-M
set textwidth=80
set ruler
set backspace=2
set guioptions+=a
set showmatch
set ic
set wrap!

map <S-Insert> <MiddleMouse>
map! <S-Insert> <MiddleMouse>
 
map <F2> :w!
map <F3> :wq!
map <F4> :q!
map! <F5> <Esc>^i// <Esc><F2>
map  <F5> ^i// <Esc><F2>
map  <F10> gf
map  <F11> :e#
map  <F6> :s/^/\/\/ /g<C-N><F2>
map  <F12> :!make
map  <F6> ^xxx

" abbreviate db dbg_msg("!cursor!");<Esc>:call search('!cursor!', 'b')<cr>cf!
" abbreviate m messagef(NORMAL, NONE, "!cursor!");<Esc>:call search('!cursor!', 'b')<cr>cf!
" abbreviate p print "!cursor!";<Esc>:call search('!cursor!', 'b')<cr>cf!
abbreviate cc /*!cursor! */<Esc>:call search('!cursor!', 'b')<cr>cf!

filetype plugin on

set t_Co=256
set pastetoggle=<F9>
set cindent
set cinkeys=0{,0},!^F,o,O,e

colo darkburn
hi LineNr guifg=Yellow ctermfg=8 ctermbg=233
hi Comment term=bold ctermfg=31 gui=italic guifg=#c0bc6c
hi Label cterm=NONE ctermfg=187 gui=underline guifg=#dfcfaf
set nu
