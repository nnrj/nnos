#include "./lib/nnos.h"

//=====8位键盘、鼠标专用先进先出缓冲区处理器========================================================
/*初始化缓冲区*/
void initFIFOBuffer(FIFO_BUFFER *fifoBuffer,unsigned char *buffer,int bufferSize){ //初始化fifo缓冲区
	fifoBuffer->buffer = buffer;
	fifoBuffer->size = bufferSize;
	fifoBuffer->free = bufferSize;
	fifoBuffer->next_write = 0;
	fifoBuffer->next_read = 0;
	fifoBuffer->flags = 0;
}

/*向缓冲区写入数据*/
int putFIFOBuffer(FIFO_BUFFER *fifoBuffer,unsigned char data){ //向先进先出缓冲区写入数据
	if(fifoBuffer->free == 0){
		fifoBuffer->flags |= FLAGS_OVERRUN; //溢出
		return -1;
	}
	fifoBuffer->buffer[fifoBuffer->next_write] = data;
	fifoBuffer->next_write = ((fifoBuffer->next_write + fifoBuffer->size + 1) % fifoBuffer->size);
	fifoBuffer->free--;
	return 0;
}

/*从缓冲区读取数据*/
int getFIFOBuffer(FIFO_BUFFER *fifoBuffer){
	int data;
	if(fifoBuffer->free == fifoBuffer->size){
		return -1;
	}
	data = fifoBuffer->buffer[fifoBuffer->next_read];
	fifoBuffer->next_read = ((fifoBuffer->next_read + fifoBuffer->size + 1) % fifoBuffer->size);
	fifoBuffer->free++;
	return data;
}

/*获取缓冲区数据数量*/
int getFIFOBufferStatus(FIFO_BUFFER *fifoBuffer){ //返回缓存区数据数量
	return fifoBuffer->size - fifoBuffer->free;
}

//=====32位通用先进先出缓冲区处理器========================================================
/*初始化32位缓冲区*/
void initFIFOBuffer32(FIFO_BUFFER32 *fifoBuffer,int *buffer,int bufferSize,TASK *task){ //初始化32位fifo缓冲区
	fifoBuffer->buffer = buffer;
	fifoBuffer->size = bufferSize;
	fifoBuffer->free = bufferSize;
	fifoBuffer->next_write = 0;
	fifoBuffer->next_read = 0;
	fifoBuffer->flags = 0;
	fifoBuffer->task = task; //初始化任务指针，无时可传0
}

/*向32位缓冲区写入数据*/
int putFIFOBuffer32(FIFO_BUFFER32 *fifoBuffer,int data){ //向先进先出缓冲区写入数据
	if(fifoBuffer->free == 0){
		fifoBuffer->flags |= FLAGS_OVERRUN; //溢出
		return -1;
	}
	fifoBuffer->buffer[fifoBuffer->next_write] = data;
	fifoBuffer->next_write = ((fifoBuffer->next_write + fifoBuffer->size + 1) % fifoBuffer->size);
	fifoBuffer->free--;
	if(fifoBuffer->task != 0){
		if(fifoBuffer->task->flag != TASK_FLAG_RUNNING){ //若任务未运行
			taskRun(fifoBuffer->task,-1,0);  //切换到当前任务，指定优先级为0，不会改变优先级，挂载道默认优先级队列
		}		
	}
	return 0;
}

/*从32位缓冲区读取数据*/
int getFIFOBuffer32(FIFO_BUFFER32 *fifoBuffer){
	int data;
	if(fifoBuffer->free == fifoBuffer->size){
		return -1;
	}
	data = fifoBuffer->buffer[fifoBuffer->next_read];
	fifoBuffer->next_read = ((fifoBuffer->next_read + fifoBuffer->size + 1) % fifoBuffer->size);
	fifoBuffer->free++;
	return data;
}

/*获取32位缓冲区数据数量*/
int getFIFOBufferStatus32(FIFO_BUFFER32 *fifoBuffer){ //返回缓存区数据数量
	return fifoBuffer->size - fifoBuffer->free;
}





