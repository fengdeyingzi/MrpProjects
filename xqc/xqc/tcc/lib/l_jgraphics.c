#include <tcc.h>
#include <mrc_jgraphics.h>


int32 l_jgraphics_init(TCCState *s)
{
	if(s==NULL)
		return -1;

	tcc_add_symbol(s,"mrc_jgraphics_init", (void*)mrc_jgraphics_init);
	tcc_add_symbol(s,"mrc_jgraphics_reinit", (void*)mrc_jgraphics_reinit);
	tcc_add_symbol(s,"mrc_jgraphics_translate", (void*)mrc_jgraphics_translate);
	tcc_add_symbol(s,"mrc_jgraphics_setClip", (void*)mrc_jgraphics_setClip);
	tcc_add_symbol(s,"mrc_jgraphics_clipRect", (void*)mrc_jgraphics_clipRect);
	tcc_add_symbol(s,"mrc_jgraphics_getClipHeight", (void*)mrc_jgraphics_getClipHeight);
	tcc_add_symbol(s,"mrc_jgraphics_getClipWidth", (void*)mrc_jgraphics_getClipWidth);
	tcc_add_symbol(s,"mrc_jgraphics_getClipX", (void*)mrc_jgraphics_getClipX);
	tcc_add_symbol(s,"mrc_jgraphics_getClipY", (void*)mrc_jgraphics_getClipY);
	tcc_add_symbol(s,"mrc_jgraphics_setFont", (void*)mrc_jgraphics_setFont);
	tcc_add_symbol(s,"mrc_jgraphics_getFont", (void*)mrc_jgraphics_getFont);
	tcc_add_symbol(s,"mrc_jgraphics_setColor", (void*)mrc_jgraphics_setColor);
	tcc_add_symbol(s,"mrc_jgraphics_setColor_Ex", (void*)mrc_jgraphics_setColor_Ex);
	tcc_add_symbol(s,"getColor_Graphics", (void*)getColor_Graphics);
	tcc_add_symbol(s,"mrc_jgraphics_drawString", (void*)mrc_jgraphics_drawString);
	tcc_add_symbol(s,"mrc_jgraphics_drawImage", (void*)mrc_jgraphics_drawImage);
	tcc_add_symbol(s,"mrc_jgraphics_drawRegion", (void*)mrc_jgraphics_drawRegion);
	tcc_add_symbol(s,"mrc_jgraphics_drawLine", (void*)mrc_jgraphics_drawLine);
	tcc_add_symbol(s,"mrc_jgraphics_fillRect", (void*)mrc_jgraphics_fillRect);
	tcc_add_symbol(s,"mrc_jgraphics_drawRect", (void*)mrc_jgraphics_drawRect);
	tcc_add_symbol(s,"mrc_jgraphics_fillRoundRect", (void*)mrc_jgraphics_fillRoundRect);
	tcc_add_symbol(s,"mrc_jgraphics_drawRoundRect", (void*)mrc_jgraphics_drawRoundRect);
	tcc_add_symbol(s,"mrc_jgraphics_drawArc", (void*)mrc_jgraphics_drawArc);
	tcc_add_symbol(s,"mrc_jgraphics_fillArc", (void*)mrc_jgraphics_fillArc);
	tcc_add_symbol(s,"mrc_jgraphics_fillTriangle", (void*)mrc_jgraphics_fillTriangle);
	

	return 0;
}