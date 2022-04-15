#include <linux/init.h>

#include <linux/module.h> 
#include "md.h" 

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Alex Prianishnikov <mrpriany@mail.ru>"); 

char* md1_data = "Hello World!"; 
int md1_int_data = 5;

extern char* md1_proc( void ) {
    return md1_data; 
} 

extern int md1_proc_square(int n) {
    return n*n;
}

static char* md1_local( void ) { 
    return md1_data; 
}
 
extern char* md1_noexport( void ) { 
    return md1_data; 
} 

EXPORT_SYMBOL( md1_data ); 
EXPORT_SYMBOL( md1_proc ); 
EXPORT_SYMBOL (md1_int_data);
EXPORT_SYMBOL (md1_proc_square);

static int __init md_init( void ) { 
    printk( "+ module md1 start!\n" ); 
    return 0; 
} 
static void __exit md_exit( void ) { 
    printk( "+ module md1 unloaded!\n" ); 
} 

module_init( md_init ); 
module_exit( md_exit );
