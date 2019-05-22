/**
 * ===================NNOS-Devices============================
 *【图层处理相关】
 * -----------------------------------------------------------
 * ·日期：2019-3-15      作者：年年软件
 * ===========================================================
**/
/*头文件引用*/
#include "./lib/nnos.h"	//系统函数声明

//=====链表版===============================================================================
/*图层初始化*/
COVER_LIST *initCoverList(MEMERY_LIST *memeryList,SCREEN screen){
	//COUNT i;
	COVER_LIST *coverList = (COVER_LIST *)memeryAlloc(memeryList,sizeof(COVER_LIST));
	if(coverList == NULL){
		return NULL;
	}
	coverList->screen = screen;
	//coverList->top = -1;
	coverList->id_max = -1;
	coverList->rear = coverList->head;
	return coverList;
}

#define SHEET_USE		1

/*图层分配*/
COVER *coverAlloc(MEMERY_LIST *memeryList,COVER_LIST *coverList){
	//int id;
	COVER *cover = (COVER *)memeryAlloc(memeryList,sizeof(COVER));
	if(cover == NULL){
		return NULL;
	} 
	cover->id = coverList->id_max + 1;
	cover->order = -1;
	coverList->id_max = cover->id;
	coverList->rear->next = cover;
	coverList->rear = cover;
	//coverList->top++;
	return cover;
}

/*图层设定*/
void setCover(COVER *cover,PICTURE picture,int transColor){
	cover->picture = picture;
	cover->trans_color = transColor;
}

/*图层状态更新*/
void updateCover(COVER_LIST *coverList,COVER *cover,int order){
	COVER *p,*q,*r;
	p = coverList->head;
	//q = p;
	//q = p;
	r = cover;
	q = r;
	COUNT oldOrder = 0;
 	if(order >= COVER_LIST_SIZE){
		cover->order = coverList->rear->order + 1;
		cover->next = coverList->rear->next;
		coverList->rear->next = cover;
		return;
	}
	else if(order <= -1){
		cover->order = -1;
		cover->next = p->next;
		p->next = cover;
		return;
	}
	else{
		while(p->next){
			if(p->next->id == cover->id){
				p->next = cover;
				if(p->next->order == order){
					return;
				}
				p->next->order = order;
				r = p->next;
			}
			if(oldOrder == order){
				q = p;	
				//break;
			}
			p=p->next;
			oldOrder++;
		}		
	}	
	r->next = q->next;
	q->next = r;
	return;
}

/*图层列表刷新*/
void coverRefresh(COVER_LIST *coverList){
	COVER *p;
	unsigned char *base;
	unsigned char *vram;
	unsigned char vcolor;
	//int px,py,scrx,scry,vx,vy;
	int px,py,vx,vy;
	p = coverList->head;
	vram = coverList->screen.vram;
	while(p->next){
		p = p->next;
		if(p->order > -1){
			base = p->picture.base;
			for(vy = 0;vy < p->picture.height;vy++){
				py = p->picture.py + vy;
				for(vx = 0;vx < p->picture.width;vx++){
					px = p->picture.px + vx;
					vcolor = base[vy * (p->picture.width) + vx];
					if(vcolor != p->trans_color){
						vram[py * coverList->screen.scrx + px] = vcolor;
					}
				}
			}			
		}
	}
}

/*图层移动*/
void coverMove(COVER_LIST *coverList,COVER *cover,int vx,int vy){ //移动图层
	cover->picture.px = vx;
	cover->picture.py = vy;
	if(cover->order >= 0){
		coverRefresh(coverList);
	}
}

/*图层释放*/
void coverFree(MEMERY_LIST *memeryList,COVER_LIST *coverList,COVER *cover){
	COVER *p,*q;
	p = coverList->head;
	if(cover->order > 0){
		updateCover(coverList,cover,-1); //隐藏
	}
	cover->flag = 0; //标记为未使用状态
	while(p->next){
		if(p->next->id == cover->id){
			q = p->next;
			p->next = q->next;
			memeryFree(memeryList,(unsigned int)&q,sizeof(q));
			break;
		}
		p = p->next;
	}
	
}

//=====老师版============================================================================
/*图层初始化*/
SHTCTL *shtctl_init(MEMERY_LIST *memman,unsigned char *vram,int xsize,int ysize){
	SHTCTL *ctl;
	int i;
	ctl = (SHTCTL *) memeryAlloc4k(memman,sizeof (SHTCTL));
	if (ctl == 0) {
		goto err;
	}
	ctl->map = (unsigned char *) memeryAlloc4k(memman,xsize * ysize);
	if (ctl->map == 0) {
		memeryFree4k(memman,(int) ctl,sizeof (SHTCTL));
		goto err;
	}
	ctl->vram = vram;
	ctl->xsize = xsize;
	ctl->ysize = ysize;
	ctl->top = -1; 
	for (i = 0; i < MAX_SHEETS; i++) {
		ctl->sheets0[i].flags = 0; 
		ctl->sheets0[i].ctl = ctl; 
	}
err:
	return ctl;
}

/*申请图层*/
/* SHEET *sheet_alloc(SHTCTL *ctl){
	SHEET *sht;
	int i;
	for (i = 0; i < MAX_SHEETS; i++) {
		if (ctl->sheets0[i].flags == 0) {
			sht = &ctl->sheets0[i];
			sht->flags = SHEET_USE;
			sht->height = -1;
			return sht;
		}
	}
	return 0;
} */
struct SHEET *sheet_alloc(SHTCTL *ctl){
	SHEET *sht;
	int i;
	/* MEMERY_LIST *memeryList = (MEMERY_LIST *)MEMERY_ADDR;
	if(memeryList->number <= 0){
		return 0;
	} */
	for (i = 0; i < MAX_SHEETS; i++) {
		if (ctl->sheets0[i].flags == 0) {
			sht = &ctl->sheets0[i];
			sht->flags = SHEET_USE;
			sht->height = -1;
			sht->task = 0; //不使用自动关闭
			return sht;
		}
	}
	return 0;
}

/*设定图层*/
void sheet_setbuf(SHEET *sht,unsigned char *buf,int xsize,int ysize,int col_inv){
	sht->buf = buf;
	sht->bxsize = xsize;
	sht->bysize = ysize;
	sht->col_inv = col_inv;
	return;
}

/*调整图层*/
void sheet_updown(SHEET *sht,int height){
	SHTCTL *ctl = sht->ctl;
	int h,old = sht->height;
	if (height > ctl->top + 1) {
		height = ctl->top + 1;
	}
	if (height < -1) {
		height = -1;
	}
	sht->height = height;
	if (old > height) {
		if (height >= 0) {
			for (h = old; h > height; h--) {
				ctl->sheets[h] = ctl->sheets[h - 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
			sheet_refreshmap(ctl,sht->vx0,sht->vy0,sht->vx0 + sht->bxsize,sht->vy0 + sht->bysize,height + 1);
			sheet_refreshsub(ctl,sht->vx0,sht->vy0,sht->vx0 + sht->bxsize,sht->vy0 + sht->bysize,height + 1,old);
		} else {
			if (ctl->top > old) {
				for (h = old; h < ctl->top; h++) {
					ctl->sheets[h] = ctl->sheets[h + 1];
					ctl->sheets[h]->height = h;
				}
			}
			ctl->top--;
			sheet_refreshmap(ctl,sht->vx0,sht->vy0,sht->vx0 + sht->bxsize,sht->vy0 + sht->bysize,0);
			sheet_refreshsub(ctl,sht->vx0,sht->vy0,sht->vx0 + sht->bxsize,sht->vy0 + sht->bysize,0,old - 1);
		}
	} else if (old < height) {
		if (old >= 0) {
			for (h = old;h < height;h++) {
				ctl->sheets[h] = ctl->sheets[h + 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
		} else {
			for (h = ctl->top; h >= height; h--) {
				ctl->sheets[h + 1] = ctl->sheets[h];
				ctl->sheets[h + 1]->height = h + 1;
			}
			ctl->sheets[height] = sht;
			ctl->top++; 
		}
		sheet_refreshmap(ctl,sht->vx0,sht->vy0,sht->vx0 + sht->bxsize,sht->vy0 + sht->bysize,height);
		sheet_refreshsub(ctl,sht->vx0,sht->vy0,sht->vx0 + sht->bxsize,sht->vy0 + sht->bysize,height,height);
	}
	return;
}


/*图层刷新*/
void sheet_refresh(SHEET *sht,int bx0,int by0,int bx1,int by1){
	if(sht->height >= 0){
		sheet_refreshsub(sht->ctl,sht->vx0 + bx0,sht->vy0 + by0,sht->vx0 + bx1,sht->vy0 + by1,sht->height,sht->height);
	}
	return;
}
/*图层刷新子函数*/
void sheet_refreshsub(SHTCTL *ctl,int vx0,int vy0,int vx1,int vy1,int h0,int h1){
	int h,bx,by,vx,vy,bx0,by0,bx1,by1;
	unsigned char *buf,*vram = ctl->vram,*map = ctl->map,sid;
	SHEET *sht;
	if(vx0 < 0){
		vx0 = 0;
	}
	if(vy0 < 0){
		vy0 = 0;
	}
	if(vx1 > ctl->xsize){
		vx1 = ctl->xsize;
	}
	if(vy1 > ctl->ysize){
		vy1 = ctl->ysize;
	}
	for(h = h0; h <= h1; h++){
		sht = ctl->sheets[h];
		buf = sht->buf;
		sid = sht - ctl->sheets0;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) {
			bx0 = 0;
		}
		if(by0 < 0){
			by0 = 0;
		}
		if(bx1 > sht->bxsize){
			bx1 = sht->bxsize;
		}
		if(by1 > sht->bysize){
			by1 = sht->bysize;
		}
		for(by = by0; by < by1; by++){
			vy = sht->vy0 + by;
			for(bx = bx0; bx < bx1; bx++){
				vx = sht->vx0 + bx;
				if (map[vy * ctl->xsize + vx] == sid){
					vram[vy * ctl->xsize + vx] = buf[by * sht->bxsize + bx];
				}
			}
		}
	}
	return;
}

/* void sheet_refreshsub(struct SHTCTL *ctl,int vx0,int vy0,int vx1,int vy1,int h0,int h1){
	int h,bx,by,vx,vy,bx0,by0,bx1,by1,bx2,sid4,i,i1,*p,*q,*r;
	unsigned char *buf,*vram = ctl->vram,*map = ctl->map,sid;
	struct SHEET *sht;
	if (vx0 < 0) { vx0 = 0; }
	if (vy0 < 0) { vy0 = 0; }
	if (vx1 > ctl->xsize) { vx1 = ctl->xsize; }
	if (vy1 > ctl->ysize) { vy1 = ctl->ysize; }
	for (h = h0; h <= h1; h++) {
		sht = ctl->sheets[h];
		buf = sht->buf;
		sid = sht - ctl->sheets0;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) { bx0 = 0; }
		if (by0 < 0) { by0 = 0; }
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }
		if (by1 > sht->bysize) { by1 = sht->bysize; }
		if ((sht->vx0 & 3) == 0) {
			i  = (bx0 + 3) / 4; 
			i1 =  bx1      / 4; 
			i1 = i1 - i;
			sid4 = sid | sid << 8 | sid << 16 | sid << 24;
			for (by = by0; by < by1; by++) {
				vy = sht->vy0 + by;
				for (bx = bx0; bx < bx1 && (bx & 3) != 0; bx++) {
					vx = sht->vx0 + bx;
					if (map[vy * ctl->xsize + vx] == sid) {
						vram[vy * ctl->xsize + vx] = buf[by * sht->bxsize + bx];
					}
				}
				vx = sht->vx0 + bx;
				p = (int *) &map[vy * ctl->xsize + vx];
				q = (int *) &vram[vy * ctl->xsize + vx];
				r = (int *) &buf[by * sht->bxsize + bx];
				for (i = 0; i < i1; i++) {
					if (p[i] == sid4) {
						q[i] = r[i];
					} else {
						bx2 = bx + i * 4;
						vx = sht->vx0 + bx2;
						if (map[vy * ctl->xsize + vx + 0] == sid) {
							vram[vy * ctl->xsize + vx + 0] = buf[by * sht->bxsize + bx2 + 0];
						}
						if (map[vy * ctl->xsize + vx + 1] == sid) {
							vram[vy * ctl->xsize + vx + 1] = buf[by * sht->bxsize + bx2 + 1];
						}
						if (map[vy * ctl->xsize + vx + 2] == sid) {
							vram[vy * ctl->xsize + vx + 2] = buf[by * sht->bxsize + bx2 + 2];
						}
						if (map[vy * ctl->xsize + vx + 3] == sid) {
							vram[vy * ctl->xsize + vx + 3] = buf[by * sht->bxsize + bx2 + 3];
						}
					}
				}
				for (bx += i1 * 4; bx < bx1; bx++) {
					vx = sht->vx0 + bx;
					if (map[vy * ctl->xsize + vx] == sid) {
						vram[vy * ctl->xsize + vx] = buf[by * sht->bxsize + bx];
					}
				}
			}
		} else {
			for (by = by0; by < by1; by++) {
				vy = sht->vy0 + by;
				for (bx = bx0; bx < bx1; bx++) {
					vx = sht->vx0 + bx;
					if (map[vy * ctl->xsize + vx] == sid) {
						vram[vy * ctl->xsize + vx] = buf[by * sht->bxsize + bx];
					}
				}
			}
		}
	}
	return;
}*/

/*图层分布*/
/* void sheet_refreshmap(SHTCTL *ctl,int vx0,int vy0,int vx1,int vy1,int h0){
	int h,bx,by,vx,vy,bx0,by0,bx1,by1;
	unsigned char *buf,sid,*map = ctl->map;
	SHEET *sht;
	if (vx0 < 0) { vx0 = 0; }
	if (vy0 < 0) { vy0 = 0; }
	if (vx1 > ctl->xsize) { vx1 = ctl->xsize; }
	if (vy1 > ctl->ysize) { vy1 = ctl->ysize; }
	for (h = h0; h <= ctl->top; h++) {
		sht = ctl->sheets[h];
		sid = sht - ctl->sheets0;
		buf = sht->buf;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) { bx0 = 0; }
		if (by0 < 0) { by0 = 0; }
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }
		if (by1 > sht->bysize) { by1 = sht->bysize; }
		for (by = by0; by < by1; by++) {
			vy = sht->vy0 + by;
			for (bx = bx0; bx < bx1; bx++) {
				vx = sht->vx0 + bx;
				if (buf[by * sht->bxsize + bx] != sht->col_inv) {
					map[vy * ctl->xsize + vx] = sid;
				}
			}
		}
	}
	return;
} */
void sheet_refreshmap(struct SHTCTL *ctl,int vx0,int vy0,int vx1,int vy1,int h0){
	int h,bx,by,vx,vy,bx0,by0,bx1,by1;
	unsigned char *buf,sid,*map = ctl->map;
	SHEET *sht;
	if(vx0 < 0){
		vx0 = 0;
	}
	if(vy0 < 0){
		vy0 = 0;
	}
	if(vx1 > ctl->xsize){
		vx1 = ctl->xsize;
	}
	if(vy1 > ctl->ysize){
		vy1 = ctl->ysize;
	}
	for(h = h0; h <= ctl->top; h++){
		sht = ctl->sheets[h];
		sid = sht - ctl->sheets0;
		buf = sht->buf;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if(bx0 < 0) { bx0 = 0; }
		if(by0 < 0) { by0 = 0; }
		if(bx1 > sht->bxsize) {
			bx1 = sht->bxsize;
		}
		if(by1 > sht->bysize){
			by1 = sht->bysize;
		}
		if(sht->col_inv == -1){ //非透明区域快速通道
			for (by = by0; by < by1; by++) {
				vy = sht->vy0 + by;
				for (bx = bx0; bx < bx1; bx++) {
					vx = sht->vx0 + bx;
					map[vy * ctl->xsize + vx] = sid;
				}
			}
		}
		else{ //透明区域普通通道
			for(by = by0; by < by1; by++){
				vy = sht->vy0 + by;
				for (bx = bx0; bx < bx1; bx++){
					vx = sht->vx0 + bx;
					if(buf[by * sht->bxsize + bx] != sht->col_inv){
						map[vy * ctl->xsize + vx] = sid;
					}
				}
			}
		}
	}
	return;
}
/* void sheet_refreshmap(struct SHTCTL *ctl,int vx0,int vy0,int vx1,int vy1,int h0)
{
	int h,bx,by,vx,vy,bx0,by0,bx1,by1,sid4,*p;
	unsigned char *buf,sid,*map = ctl->map;
	struct SHEET *sht;
	if (vx0 < 0) { vx0 = 0; }
	if (vy0 < 0) { vy0 = 0; }
	if (vx1 > ctl->xsize) { vx1 = ctl->xsize; }
	if (vy1 > ctl->ysize) { vy1 = ctl->ysize; }
	for (h = h0; h <= ctl->top; h++) {
		sht = ctl->sheets[h];
		sid = sht - ctl->sheets0;
		buf = sht->buf;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) { bx0 = 0; }
		if (by0 < 0) { by0 = 0; }
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }
		if (by1 > sht->bysize) { by1 = sht->bysize; }
		if (sht->col_inv == -1) { //非透明区域快速通道（4字节），利用16位寄存器代替8位执行MOV，相邻地址会同时写入数据，32位则1条指令同时向4个地址写入值
			if ((sht->vx0 & 3) == 0 && (bx0 & 3) == 0 && (bx1 & 3) == 0) {
				bx1 = (bx1 - bx0) / 4;  //MOV次数
				sid4 = sid | sid << 8 | sid << 16 | sid << 24;
				for (by = by0; by < by1; by++) {
					vy = sht->vy0 + by;
					vx = sht->vx0 + bx0;
					p = (int *) &map[vy * ctl->xsize + vx];
					for (bx = 0; bx < bx1; bx++) {
						p[bx] = sid4;
					}
				}
			} else { //非透明区域快速通道（1字节）
				for (by = by0; by < by1; by++) {
					vy = sht->vy0 + by;
					for (bx = bx0; bx < bx1; bx++) {
						vx = sht->vx0 + bx;
						map[vy * ctl->xsize + vx] = sid;
					}
				}
			}
		} else { //透明区域普通通道
			for (by = by0; by < by1; by++) {
				vy = sht->vy0 + by;
				for (bx = bx0; bx < bx1; bx++) {
					vx = sht->vx0 + bx;
					if (buf[by * sht->bxsize + bx] != sht->col_inv) {
						map[vy * ctl->xsize + vx] = sid;
					}
				}
			}
		}
	}
	return;
} */



/*图层移动*/
void sheet_slide(SHEET *sht,int vx0,int vy0){
	SHTCTL *ctl = sht->ctl;
	int old_vx0 = sht->vx0,old_vy0 = sht->vy0;
	sht->vx0 = vx0;
	sht->vy0 = vy0;
	if (sht->height >= 0) {
		sheet_refreshmap(ctl,old_vx0,old_vy0,old_vx0 + sht->bxsize,old_vy0 + sht->bysize,0);
		sheet_refreshmap(ctl,vx0,vy0,vx0 + sht->bxsize,vy0 + sht->bysize,sht->height);
		sheet_refreshsub(ctl,old_vx0,old_vy0,old_vx0 + sht->bxsize,old_vy0 + sht->bysize,0,sht->height - 1);
		sheet_refreshsub(ctl,vx0,vy0,vx0 + sht->bxsize,vy0 + sht->bysize,sht->height,sht->height);
	}
	return;
}

/*图层释放*/
void sheet_free(SHEET *sht){
	if (sht->height >= 0) {
		sheet_updown(sht,-1); 
	}
	sht->flags = 0;
	return;
}

//=====数组版============================================================================
/*图层初始化*/
/* COVER_LIST *initCoverList(MEMERY_LIST *memeryList,SCREEN screen){
	COVER_LIST *coverList;
	COUNT i;
	coverList = (COVER_LIST *)memeryList(memeryList,sizeof(COVER_LIST));
	if(coverList == 0){
		return coverList;
	}
	coverList->screen = screen;
	coverList->top = -1;
	for(i = 0;i < COVER_LIST_SIZE;i++){
		coverList->covers0[i].flag = COVER_UNUSE; //所有图层标记为未使用
	}
} */

/*图层分配*/
/* COVER *coverAlloc(COVER_LIST *coverList){
	COVER *cover;
	COUNT i;
	for(i = 0,i < COVER_LIST_SIZE;i++){
		if(coverList->covers0[i].flag == 0){
			cover = &coverList->covers0[i];
			cover->flag = COVER_USEED; //标记为已使用
			cover->order = -1; //级别为-1，隐藏不显示
			return cover;
		}
	}
	return 0; //无空闲图层，分配失败
} */

/*图层设定*/
/* void setCover(COVER *cover,PICTURE picture,int trans_color){ //设定图层
	cover->picture = picture;
	cover->trans_color = trans_color;
} */

/*图层调整*/
/* void setCoverOrder(COVER_LIST *coverList,COVER *cover,int order){
	int oldOrder = cover->order;
	COUNT i;
	if(order > cover->top + 1){
		order = cover->top + 1;
	}
	if(order < -1){
		order = -1;
	}
	cover->order = order;
	if(order < oldOrder){
		if(orderr >= 0){
			for(i = oldOrder;i > order;i--){ //中间上升
				coverList->covers[i] = coverList->[i - 1];
				coverList->covers[i]->order = i; 
			}
			coverList->covers[order] = cover;
		}
		else{
			if(coverList->top > oldOrder){
				for(i = oldOrder;i < coverList->top;i++){ //顶部下降
					coverList->covers[i] = coverList->covers[i + 1];
					coverList->covers[i]->order = i;
				}
			}
			coverList->top--;
		}
		coverRefresh(coverList); //刷新屏幕图层
	}
	else if(order > oldOrder){
		if(oldOrder > 0){
			for(i = oldOrder;i < order;i++){ //中间下降
				coverList->covers[i] = coverList->covers[i + 1];
				coverList->covers[i]->order = i;
			}
			coverList->covers[order] = cover; 
		}
		else{
			for(i = coverList->top;i >= order;i++){ //上面上升
				coverList->covers[i + 1] = coverList->covers[i];
				coverList->covers[i + 1]->order = i + 1;
			}
			coverList->covers[order] = cover;
			cover->top++;
		}
		coverRefresh(coverList);
	}
} */

/*图层刷新*/ 
/* void coverRefresh(COVER_LIST *coverList){ //刷新图层
	int i,px,py,srcx,srcy;
	unsigned char *base;
	unsigned char *vram;
	unsigned char *vcolor;
	COVER *cover;
	vram = coverList -> screen.vram;
	
	for(i = 0;i <= coverList->top;i++){
		cover = coverList->covers[i];
		base = cover->picture.pic_base;
		for(py = 0;py < cover->picture.pic_height;py++){
			vy = cover->picture.pic_Y + py;
			for(px = 0;px < cover->picture.pic_width;px++){
				vx = cover->picture.pic_X + px;
				vcolor = base[py * cover->picture.width + px];
				if(vcolor != cover->trans_color){
					vram[vy * coverList->screen.scrx + vx] = vcolor;
				}
			}
		}
	}
} */

/*图层移动*/
/* void coverMove(COVER_LIST *coverList,COVER *cover,int vx,int vy){ //移动图层
	cover->picture.pic_X = vx;
	cover->picture.pic_Y = vy;
	if(cover->order >= 0){
		coverRefresh(coverList);
	}
}*/

/*图层释放*/
/*void coverFree(COVER_LIST *coverList,COVER *cover){
	if(cover->order > 0){
		setCoverOrder(coverList,cover,-1); //隐藏
	}
	cover->flag = 0; //标记为未使用状态
} */


