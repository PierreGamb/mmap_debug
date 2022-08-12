
/****************************************************************************
 ****************************************************************************/
/** Copyright (C) 2014-2015 Xilinx, Inc.  All rights reserved.
 ** Permission is hereby granted, free of charge, to any person obtaining
 ** a copy of this software and associated documentation files (the
 ** "Software"), to deal in the Software without restriction, including
 ** without limitation the rights to use, copy, modify, merge, publish,
 ** distribute, sublicense, and/or sell copies of the Software, and to
 ** permit persons to whom the Software is furnished to do so, subject to
 ** the following conditions:
 ** The above copyright notice and this permission notice shall be included
 ** in all copies or substantial portions of the Software.Use of the Software 
 ** is limited solely to applications: (a) running on a Xilinx device, or 
 ** (b) that interact with a Xilinx device through a bus or interconnect.  
 ** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 ** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 ** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 ** NONINFRINGEMENT. IN NO EVENT SHALL XILINX BE LIABLE FOR ANY
 ** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 ** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 ** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ** Except as contained in this notice, the name of the Xilinx shall
 ** not be used in advertising or otherwise to promote the sale, use or other
 ** dealings in this Software without prior written authorization from Xilinx
 **/
/*****************************************************************************
*****************************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/pagemap.h>	
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/cdev.h>

#define MYNAME   "Raw Data 0"
#define DEV_NAME  "xraw_data0"

#define PAGE_ORDER 0
#define FRAME_BUFFER "/dev/fb0"

struct cdev *xrawCdev=NULL;
struct file_operations xrawDevFileOps;
struct page *page;

static void mod_mmap(struct file *file,struct vm_area_struct *vma)// pour récupérer le frame buffer contenant l'image 
{
printk("call mmap\n");
    struct mm_struct *mm;
    unsigned long size;
    unsigned long pfn_start;
    void *virt_start;
    int ret;

int MAX_SIZE = 100;


    mm = current->mm;
    pfn_start = page_to_pfn(page) + vma->vm_pgoff;
    virt_start = page_address(page) + (vma->vm_pgoff << PAGE_SHIFT);

    size = min(((1 << PAGE_ORDER) - vma->vm_pgoff) << PAGE_SHIFT,
               vma->vm_end - vma->vm_start);

    printk("phys_start: 0x%lx, offset: 0x%lx, vma_size: 0x%lx, map size:0x%lx\n",
           pfn_start << PAGE_SHIFT, vma->vm_pgoff << PAGE_SHIFT,
           vma->vm_end - vma->vm_start, size);

    if (size <= 0) {
        printk("%s: offset 0x%lx too large, max size is 0x%lx\n", __func__,
               vma->vm_pgoff << PAGE_SHIFT, MAX_SIZE);
        return -EINVAL;
    }

    ret = remap_pfn_range(vma, vma->vm_start, pfn_start, size, vma->vm_page_prot);

    if (ret) {
        printk("remap_pfn_range failed, vm_start: 0x%lx\n", vma->vm_start);
    }
    else {
        printk("map kernel 0x%px to user 0x%lx, size: 0x%lx\n",
               virt_start, vma->vm_start, size);
	}
	
}


#ifndef XILINX_PCIE_EP
static 
#endif
int host_pump_driver_init(void)
{
	int chrRet;
	dev_t xrawDev;
	//static struct file_operations xrawDevFileOps;
	int retval = 0;
	int i;

	/* First allocate a major/minor number. */
	//chrRet = alloc_chrdev_region (&xrawDev, 0, 1, DEV_NAME);
	chrRet = alloc_chrdev_region (&xrawDev, 0, 1, FRAME_BUFFER);
	if (chrRet < 0)
	{
		printk(KERN_ERR "Error allocating char device region\n");
		return -1;
	}
	else
	{
		/* Register our character device */
		xrawCdev = cdev_alloc ();
		if (IS_ERR (xrawCdev))
		{
			printk(KERN_ERR "Alloc error registering device driver\n");
			unregister_chrdev_region (xrawDev, 1);
			return -1;
		}
		else
		{
printk("file ops link modules start \n");
			xrawDevFileOps.owner = THIS_MODULE;
			xrawDevFileOps.mmap = mod_mmap;
printk("file ops link modules end \n");
			xrawCdev->owner = THIS_MODULE;
			xrawCdev->ops = &xrawDevFileOps; // Association des appels de modules ci-dessus et du fichier d'échange dans xrawCdev
			xrawCdev->dev = xrawDev;
			chrRet = cdev_add (xrawCdev, xrawDev, 1);
printk("file ops link modules end2 \n");
			if (chrRet < 0)
			{
				printk (KERN_ERR "Add error registering device driver\n");
				cdev_del(xrawCdev);
				unregister_chrdev_region (xrawDev, 1);
				return -1;
			}
		}
	}

	if (!IS_ERR ((int *) chrRet))
	{
		printk (KERN_INFO "Device registered with major number %d\n",
				MAJOR (xrawDev));

	}
	
	page=alloc_pages(GFP_KERNEL,PAGE_ORDER);
	printk("page physical address : %lx\n",page_to_pfn(page)<<PAGE_SHIFT);

	printk(KERN_ERR"\n Module Host pump on all 4 channels loaded %d", retval);
	return 0;
}

static void host_pump_driver_exit(void)
{
		cdev_del(xrawCdev);
		unregister_chrdev_region(xrawCdev->dev,1);

}

module_init(host_pump_driver_init);
module_exit(host_pump_driver_exit);

MODULE_DESCRIPTION("Xilinx PS PCIe DMA driver");
MODULE_AUTHOR("Xilinx");
MODULE_LICENSE("GPL");
