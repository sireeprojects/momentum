/* Check GCC version, just to be safe */
#if !defined(__GNUC__) || (__GNUC__ < 5) || (__GNUC_MINOR__ < 1)
#error atomic.h works only with GCC newer than version 4.1
#endif /* GNUC >= 4.1 */


int main() {

}
