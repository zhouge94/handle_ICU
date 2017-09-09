#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#define NOKEY 0

int main()
{
	int keys_fd;	
        char ret[2]; 
	struct input_event t;
	char *dev;

        setvbuf(stdout, (char *)NULL, _IONBF, 0);//disable stdio out buffer;
//	dev = getenv("KEYPAD_DEV");
      
        keys_fd = open("/dev/input/event0", O_RDONLY);
	if(keys_fd<=0)
	{
                printf("open %s device error!\n",dev);
		return 0;
	}

	while(1)
	{	
                if(read(keys_fd,&t,sizeof(t))==sizeof(t)) {
		    if(t.type==EV_KEY)
			if(t.value==0 || t.value==1)
			{
			//	printf("%d \n", t.code);
				switch(t.code)
				{
					case 116:
			    		printf("key116 %s\n",(t.value)?"Presse":"Released");
			    	break;
			    	
			    	case 158:
			    		printf("key158 %s\n",(t.value)?"Pressed":"Released");
			    	break;
			    	
			    	case 115:
			    		printf("key115 %s\n",(t.value)?"pressed":"Released");
			    	break;  
			    	
			    	case 114:
			    		printf("key114 %s\n",(t.value)?"Pressed":"Released");
			    	break;
			    	
			    	case 102:
			    	printf("key102 %s\n",(t.value)?"Released":"Pressed");
			    	break;

					//case 261:
					//	printf("key261 %s\n",(t.value)?"Released":"Pressed");
					//break;
			    	
			    	default:
			    		break;
			    }
			}
		}
	}	
	close(keys_fd);
	
        return 0;
}

