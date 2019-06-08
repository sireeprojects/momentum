Insert a 'C' comment at current position of the cursor in 'insert' mode
abbreviate /* /*!cursor! */<Esc>:call search('!cursor!', 'b')<cr>cf!

Comment a line in 'C' style in command mode
map <F5> <Esc>^i/* <Esc> ^A */<Esc><F2>

Comment a line in 'C' style in insert mode
map! <F5> <Esc>^i/* <Esc> ^A */<Esc><F2>

Uncomment a line with 'C' style comment
map <F6> ^dw^A<Esc>xxx
*/


-- highlight characters that go beyond 80 char per line
:match ErrorMsg /\%>80v.\+/

-- honor textwidth
-- j when usign ctrl+j remove leading comment //
set formatoptions+=tcj


gvim full screen
yum install -y wmctrl

f11 shortcut for toggle gvim fullscreen
map <silent> <F11> :call system("wmctrl -ir " . v:windowid . " -b toggle,fullscreen")<CR>
