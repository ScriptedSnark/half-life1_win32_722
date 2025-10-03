 .386P
 .model FLAT
 externdef _d_zistepu:dword
 externdef _d_pzbuffer:dword
 externdef _d_zistepv:dword
 externdef _d_zrowbytes:dword
 externdef _d_ziorigin:dword
 externdef _r_turb_s:dword
 externdef _r_turb_t:dword
 externdef _r_turb_pdest:dword
 externdef _r_turb_spancount:dword
 externdef _r_turb_turb:dword
 externdef _r_turb_pbase:dword
 externdef _r_turb_sstep:dword
 externdef _r_turb_tstep:dword
 externdef _r_bmodelactive:dword
 externdef _d_sdivzstepu:dword
 externdef _d_tdivzstepu:dword
 externdef _d_sdivzstepv:dword
 externdef _d_tdivzstepv:dword
 externdef _d_sdivzorigin:dword
 externdef _d_tdivzorigin:dword
 externdef _sadjust:dword
 externdef _tadjust:dword
 externdef _bbextents:dword
 externdef _bbextentt:dword
 externdef _cacheblock:dword
 externdef _d_viewbuffer:dword
 externdef _cachewidth:dword
 externdef _d_pzbuffer:dword
 externdef _d_zrowbytes:dword
 externdef _d_zwidth:dword
 externdef _d_scantable:dword
 externdef _r_lightptr:dword
 externdef _r_numvblocks:dword
 externdef _prowdestbase:dword
 externdef _pbasesource:dword
 externdef _r_lightwidth:dword
 externdef _lightright:dword
 externdef _lightrightstep:dword
 externdef _lightdeltastep:dword
 externdef _lightdelta:dword
 externdef _lightright:dword
 externdef _lightdelta:dword
 externdef _sourcetstep:dword
 externdef _surfrowbytes:dword
 externdef _lightrightstep:dword
 externdef _lightdeltastep:dword
 externdef _r_sourcemax:dword
 externdef _r_stepback:dword
 externdef _colormap:dword
 externdef _blocksize:dword
 externdef _sourcesstep:dword
 externdef _lightleft:dword
 externdef _blockdivshift:dword
 externdef _blockdivmask:dword
 externdef _lightleftstep:dword
 externdef _r_origin:dword
 externdef _r_ppn:dword
 externdef _r_pup:dword
 externdef _r_pright:dword
 externdef _ycenter:dword
 externdef _xcenter:dword
 externdef _d_vrectbottom_particle:dword
 externdef _d_vrectright_particle:dword
 externdef _d_vrecty:dword
 externdef _d_vrectx:dword
 externdef _d_pix_shift:dword
 externdef _d_pix_min:dword
 externdef _d_pix_max:dword
 externdef _d_y_aspect_shift:dword
 externdef _screenwidth:dword
 externdef _vright:dword
 externdef _vup:dword
 externdef _vpn:dword
 externdef _BOPS_Error:dword
 externdef _snd_scaletable:dword
 externdef _paintbuffer:dword
 externdef _snd_linear_count:dword
 externdef _snd_p:dword
 externdef _snd_vol:dword
 externdef _snd_out:dword
 externdef _r_leftclipped:dword
 externdef _r_leftenter:dword
 externdef _r_rightclipped:dword
 externdef _r_rightenter:dword
 externdef _modelorg:dword
 externdef _xscale:dword
 externdef _r_refdef:dword
 externdef _yscale:dword
 externdef _r_leftexit:dword
 externdef _r_rightexit:dword
 externdef _r_lastvertvalid:dword
 externdef _cacheoffset:dword
 externdef _newedges:dword
 externdef _removeedges:dword
 externdef _r_pedge:dword
 externdef _r_framecount:dword
 externdef _r_u1:dword
 externdef _r_emitted:dword
 externdef _edge_p:dword
 externdef _surface_p:dword
 externdef _surfaces:dword
 externdef _r_lzi1:dword
 externdef _r_v1:dword
 externdef _r_ceilv1:dword
 externdef _r_nearzi:dword
 externdef _r_nearzionly:dword
 externdef _edge_aftertail:dword
 externdef _edge_tail:dword
 externdef _current_iv:dword
 externdef _edge_head_u_shift20:dword
 externdef _span_p:dword
 externdef _edge_head:dword
 externdef _fv:dword
 externdef _edge_tail_u_shift20:dword
 externdef _r_apverts:dword
 externdef _r_anumverts:dword
 externdef _aliastransform:dword
 externdef _r_avertexnormals:dword
 externdef _r_plightvec:dword
 externdef _r_ambientlight:dword
 externdef _r_shadelight:dword
 externdef _aliasxcenter:dword
 externdef _aliasycenter:dword
 externdef _a_sstepxfrac:dword
 externdef _r_affinetridesc:dword
 externdef _acolormap:dword
 externdef _d_pcolormap:dword
 externdef _r_affinetridesc:dword
 externdef _d_sfrac:dword
 externdef _d_ptex:dword
 externdef _d_pedgespanpackage:dword
 externdef _d_tfrac:dword
 externdef _d_light:dword
 externdef _d_zi:dword
 externdef _d_pdest:dword
 externdef _d_pz:dword
 externdef _d_aspancount:dword
 externdef _erroradjustup:dword
 externdef _errorterm:dword
 externdef _d_xdenom:dword
 externdef _r_p0:dword
 externdef _r_p1:dword
 externdef _r_p2:dword
 externdef _a_tstepxfrac:dword
 externdef _r_sstepx:dword
 externdef _r_tstepx:dword
 externdef _a_ststepxwhole:dword
 externdef _zspantable:dword
 externdef _skintable:dword
 externdef _r_zistepx:dword
 externdef _erroradjustdown:dword
 externdef _d_countextrastep:dword
 externdef _ubasestep:dword
 externdef _a_ststepxwhole:dword
 externdef _a_tstepxfrac:dword
 externdef _r_lstepx:dword
 externdef _a_spans:dword
 externdef _erroradjustdown:dword
 externdef _d_pdestextrastep:dword
 externdef _d_pzextrastep:dword
 externdef _d_sfracextrastep:dword
 externdef _d_ptexextrastep:dword
 externdef _d_countextrastep:dword
 externdef _d_tfracextrastep:dword
 externdef _d_lightextrastep:dword
 externdef _d_ziextrastep:dword
 externdef _d_pdestbasestep:dword
 externdef _d_pzbasestep:dword
 externdef _d_sfracbasestep:dword
 externdef _d_ptexbasestep:dword
 externdef _ubasestep:dword
 externdef _d_tfracbasestep:dword
 externdef _d_lightbasestep:dword
 externdef _d_zibasestep:dword
 externdef _zspantable:dword
 externdef _r_lstepy:dword
 externdef _r_sstepy:dword
 externdef _r_tstepy:dword
 externdef _r_zistepy:dword
 externdef _D_PolysetSetEdgeTable:dword
 externdef _D_RasterizeAliasPolySmooth:dword
 externdef float_point5:dword
 externdef Float2ToThe31nd:dword
 externdef izistep:dword
 externdef izi:dword
 externdef FloatMinus2ToThe31nd:dword
 externdef float_1:dword
 externdef float_particle_z_clip:dword
 externdef float_minus_1:dword
 externdef float_0:dword
 externdef fp_16:dword
 externdef fp_64k:dword
 externdef fp_1m:dword
 externdef fp_1m_minus_1:dword
 externdef fp_8:dword
 externdef entryvec_table:dword
 externdef advancetable:dword
 externdef sstep:dword
 externdef tstep:dword
 externdef pspantemp:dword
 externdef counttemp:dword
 externdef jumptemp:dword
 externdef reciprocal_table:dword
 externdef DP_Count:dword
 externdef DP_u:dword
 externdef DP_v:dword
 externdef DP_32768:dword
 externdef DP_Color:dword
 externdef DP_Pix:dword
 externdef DP_EntryTable:dword
 externdef pbase:dword
 externdef s:dword
 externdef t:dword
 externdef sfracf:dword
 externdef tfracf:dword
 externdef snext:dword
 externdef tnext:dword
 externdef spancountminus1:dword
 externdef zi16stepu:dword
 externdef sdivz16stepu:dword
 externdef tdivz16stepu:dword
 externdef zi8stepu:dword
 externdef sdivz8stepu:dword
 externdef tdivz8stepu:dword
 externdef reciprocal_table_16:dword
 externdef entryvec_table_16:dword
 externdef ceil_cw:dword
 externdef single_cw:dword
 externdef fp_64kx64k:dword
 externdef pz:dword
 externdef spr8entryvec_table:dword
 externdef _r_palette:dword
 externdef _r_lut:byte
 externdef _r_icolormix:dword
 externdef _r_blend:dword
_DATA SEGMENT
 align 4
p10_minus_p20 dd 0
p01_minus_p21 dd 0
temp0 dd 0
temp1 dd 0
Ltemp dd 0
 public _is15bit	; TODO: Temporary
 _is15bit dd 0		; TODO: Temporary
aff8entryvec_table dd LDraw8, LDraw7, LDraw6, LDraw5
 dd LDraw4, LDraw3, LDraw2, LDraw1
lzistepx dd 0
_DATA ENDS
_TEXT SEGMENT
 externdef _D_PolysetSetEdgeTable:dword
 externdef _D_RasterizeAliasPolySmooth:dword
 externdef _hlRGB:dword
 public _D_PolysetCalcGradients
_D_PolysetCalcGradients:
 fild ds:dword ptr[_r_p0+0]
 fild ds:dword ptr[_r_p2+0]
 fild ds:dword ptr[_r_p0+4]
 fild ds:dword ptr[_r_p2+4]
 fild ds:dword ptr[_r_p1+0]
 fild ds:dword ptr[_r_p1+4]
 fxch st(3)
 fsub st(0),st(2)
 fxch st(1)
 fsub st(0),st(4)
 fxch st(5)
 fsubrp st(4),st(0)
 fxch st(2)
 fsubrp st(1),st(0)
 fxch st(1)
 fld ds:dword ptr[_d_xdenom]
 fxch st(4)
 fstp ds:dword ptr[p10_minus_p20]
 fstp ds:dword ptr[p01_minus_p21]
 fxch st(2)
 fild ds:dword ptr[_r_p2+16]
 fild ds:dword ptr[_r_p0+16]
 fild ds:dword ptr[_r_p1+16]
 fxch st(2)
 fld st(0)
 fsubp st(2),st(0)
 fsubp st(2),st(0)
 fld st(0)
 fmul st(0),st(5)
 fxch st(2)
 fld st(0)
 fmul ds:dword ptr[p01_minus_p21]
 fxch st(2)
 fmul ds:dword ptr[p10_minus_p20]
 fxch st(1)
 fmul st(0),st(5)
 fxch st(2)
 fsubrp st(3),st(0)
 fsubp st(1),st(0)
 fld st(2)
 fmul ds:dword ptr[float_minus_1]
 fxch st(2)
 fmul st(0),st(3)
 fxch st(1)
 fmul st(0),st(2)
 fldcw ds:word ptr[ceil_cw]
 fistp ds:dword ptr[_r_lstepy]
 fistp ds:dword ptr[_r_lstepx]
 fldcw ds:word ptr[single_cw]
 fild ds:dword ptr[_r_p2+8]
 fild ds:dword ptr[_r_p0+8]
 fild ds:dword ptr[_r_p1+8]
 fxch st(2)
 fld st(0)
 fsubp st(2),st(0)
 fsubp st(2),st(0)
 fld st(0)
 fmul st(0),st(6)
 fxch st(2)
 fld st(0)
 fmul ds:dword ptr[p01_minus_p21]
 fxch st(2)
 fmul ds:dword ptr[p10_minus_p20]
 fxch st(1)
 fmul st(0),st(6)
 fxch st(2)
 fsubrp st(3),st(0)
 fsubp st(1),st(0)
 fmul st(0),st(2)
 fxch st(1)
 fmul st(0),st(3)
 fxch st(1)
 fistp ds:dword ptr[_r_sstepy]
 fistp ds:dword ptr[_r_sstepx]
 fild ds:dword ptr[_r_p2+12]
 fild ds:dword ptr[_r_p0+12]
 fild ds:dword ptr[_r_p1+12]
 fxch st(2)
 fld st(0)
 fsubp st(2),st(0)
 fsubp st(2),st(0)
 fld st(0)
 fmul st(0),st(6)
 fxch st(2)
 fld st(0)
 fmul ds:dword ptr[p01_minus_p21]
 fxch st(2)
 fmul ds:dword ptr[p10_minus_p20]
 fxch st(1)
 fmul st(0),st(6)
 fxch st(2)
 fsubrp st(3),st(0)
 fsubp st(1),st(0)
 fmul st(0),st(2)
 fxch st(1)
 fmul st(0),st(3)
 fxch st(1)
 fistp ds:dword ptr[_r_tstepy]
 fistp ds:dword ptr[_r_tstepx]
 fild ds:dword ptr[_r_p2+20]
 fild ds:dword ptr[_r_p0+20]
 fild ds:dword ptr[_r_p1+20]
 fxch st(2)
 fld st(0)
 fsubp st(2),st(0)
 fsubp st(2),st(0)
 fld st(0)
 fmulp st(6),st(0)
 fxch st(1)
 fld st(0)
 fmul ds:dword ptr[p01_minus_p21]
 fxch st(2)
 fmul ds:dword ptr[p10_minus_p20]
 fxch st(1)
 fmulp st(5),st(0)
 fxch st(5)
 fsubp st(1),st(0)
 fxch st(3)
 fsubrp st(4),st(0)
 fxch st(1)
 fmulp st(2),st(0)
 fmulp st(2),st(0)
 fistp ds:dword ptr[_r_zistepx]
 fistp ds:dword ptr[_r_zistepy]
 mov eax,ds:dword ptr[_r_sstepx]
 mov edx,ds:dword ptr[_r_tstepx]
 shl eax,16
 shl edx,16
 mov ds:dword ptr[_a_sstepxfrac],eax
 mov ds:dword ptr[_a_tstepxfrac],edx
 mov ecx,ds:dword ptr[_r_sstepx]
 mov eax,ds:dword ptr[_r_tstepx]
 sar ecx,16
 sar eax,16
 imul ds:dword ptr[4+0+esp]
 add eax,ecx
 mov ds:dword ptr[_a_ststepxwhole],eax
 ret
 public _D_PolysetRecursiveTriangle
_D_PolysetRecursiveTriangle:
 push ebp
 push esi
 push edi
 push ebx
 mov esi, ds:[esp+24]
 mov ebx, ds:[esp+20]
 mov edi, ds:[esp+28]
 mov eax, [esi]
 mov edx, [ebx]
 mov ebp, [esi+4]
 sub eax, edx
 mov ecx, [ebx+4]
 sub ebp, ecx
 inc eax
 cmp eax, 2
 ja short LSplit
 mov eax, [edi]
 inc ebp
 cmp ebp, 2
 ja short LSplit
 mov edx, [esi]
 mov ebp, [edi+4]
 sub eax, edx
 mov ecx, [esi+4]
 sub ebp, ecx
 inc eax
 cmp eax, 2
 ja short LSplit2
 mov eax, [ebx]
 inc ebp
 cmp ebp, 2
 ja short LSplit2
 mov edx, [edi]
 mov ebp, [ebx+4]
 sub eax, edx
 mov ecx, [edi+4]
 sub ebp, ecx
 inc eax
 inc ebp
 mov edx, ebx
 cmp eax, 2
 ja short LSplit3
 cmp ebp, 2
 jbe LDone
LSplit3:
 mov ebx, edi
 mov edi, esi
 mov esi, edx
 jmp short LSplit
LSplit2:
 mov eax, ebx
 mov ebx, esi
 mov esi, edi
 mov edi, eax
LSplit:
 sub esp, 24
 mov eax, [ebx+8]
 mov edx, [esi+8]
 mov ecx, [ebx+12]
 add eax, edx
 mov edx, [esi+12]
 sar eax, 1
 add ecx, edx
 mov ds:[esp+8], eax
 mov eax, [ebx+20]
 sar ecx, 1
 mov edx, [esi+20]
 mov ds:[esp+12], ecx
 add eax, edx
 mov ecx, [ebx]
 mov edx, [esi]
 sar eax, 1
 add edx, ecx
 mov ds:[esp+20], eax
 mov eax, [ebx+4]
 sar edx, 1
 mov ebp, [esi+4]
 mov ds:[esp], edx
 add ebp, eax
 sar ebp, 1
 mov ds:[esp+4], ebp
 cmp [esi+4], eax
 jg LNoDraw
 mov edx, [esi]
 jnz short LDraw
 cmp edx, ecx
 jl short LNoDraw
LDraw:
 mov edx, ds:[esp+20]
 mov ecx, ds:[esp+4]
 sar edx, 16
 mov ebp, ds:[esp]
 mov eax, _zspantable[ecx*4]
 cmp dx, [eax+ebp*2]
 jl short LNoDraw
 mov [eax+ebp*2], dx
 mov eax, ds:[esp+0Ch]
 sar eax, 16
 mov edx, ds:[esp+8]
 sar edx, 16
 sub ecx, ecx
 mov eax, _skintable[eax*4]
 mov ebp, ds:[esp+4]
 mov cl, [edx+eax]
 mov edx, _d_pcolormap
 xor eax, eax
 mov al, [ecx+edx]
 push eax
 mov eax, _r_palette
 push eax
 call near ptr _hlRGB
 add esp, 8
 mov edx, eax
 mov ecx, ds:[esp]
 add ecx, ecx
 mov eax, _d_scantable[ebp*4]
 add ecx, eax
 mov eax, _d_viewbuffer
 mov [eax+ecx], dx
LNoDraw:
 push esp
 push ebx
 push edi
 call near ptr _D_PolysetRecursiveTriangle
 mov ebx, esp
 push esi
 push ebx
 push edi
 call near ptr _D_PolysetRecursiveTriangle
 add esp, 24
LDone:
 pop ebx
 pop edi
 pop esi
 pop ebp
 ret 12
 public _D_PolysetAff8Start
_D_PolysetAff8Start:
 public __D_PolysetDrawSpans8
__D_PolysetDrawSpans8:
 push esi
 push ebx
 mov esi,ds:dword ptr[4+8+esp]
 mov ecx,ds:dword ptr[_r_zistepx]
 push ebp
 push edi
 ror ecx,16
 mov edx,ds:dword ptr[8+esi]
 mov ds:dword ptr[lzistepx],ecx
LSpanLoop:
 mov eax,ds:dword ptr[_d_aspancount]
 sub eax,edx
 mov edx,ds:dword ptr[_erroradjustup]
 mov ebx,ds:dword ptr[_errorterm]
 add ebx,edx
 js LNoTurnover
 mov edx,ds:dword ptr[_erroradjustdown]
 mov edi,ds:dword ptr[_d_countextrastep]
 sub ebx,edx
 mov ebp,ds:dword ptr[_d_aspancount]
 mov ds:dword ptr[_errorterm],ebx
 add ebp,edi
 mov ds:dword ptr[_d_aspancount],ebp
 jmp LRightEdgeStepped
LNoTurnover:
 mov edi,ds:dword ptr[_d_aspancount]
 mov edx,ds:dword ptr[_ubasestep]
 mov ds:dword ptr[_errorterm],ebx
 add edi,edx
 mov ds:dword ptr[_d_aspancount],edi
LRightEdgeStepped:
 cmp eax,1
 jl LNextSpan
 jz LExactlyOneLong
 mov ecx,ds:dword ptr[_a_ststepxwhole]
 mov edx,ds:dword ptr[_r_affinetridesc+8]
 mov ds:dword ptr[advancetable+4],ecx
 add ecx,edx
 mov ds:dword ptr[advancetable],ecx
 mov ecx,ds:dword ptr[_a_tstepxfrac]
 mov cx,ds:word ptr[_r_lstepx]
 mov edx,eax
 mov ds:dword ptr[tstep],ecx
 add edx,7
 shr edx,3
 mov ebx,ds:dword ptr[16+esi]
 mov bx,dx
 mov ecx,ds:dword ptr[4+esi]
 neg eax
 mov edi,ds:dword ptr[0+esi]
 and eax,7
 sub edi,eax
 sub ecx,eax
 sub ecx,eax
 mov edx,ds:dword ptr[20+esi]
 mov dx,ds:word ptr[24+esi]
 mov ebp,ds:dword ptr[28+esi]
 ror ebp,16
 push esi
 mov esi,ds:dword ptr[12+esi]
 jmp dword ptr[aff8entryvec_table+eax*4]
LDrawLoop:
LDraw8:
 cmp bp,ds:word ptr[ecx]
 jl Lp1
 xor eax,eax
 mov ah,dh
 mov al,ds:byte ptr[esi]
 mov ds:word ptr[ecx],bp
LPatch8:
 mov al,ds:byte ptr[12345678h+eax]
 mov ds:byte ptr[edi],al
Lp1:
 add edx,ds:dword ptr[tstep]
 sbb eax,eax
 add ebp,ds:dword ptr[lzistepx]
 adc ebp,0
 add ebx,ds:dword ptr[_a_sstepxfrac]
 adc esi,ds:dword ptr[advancetable+4+eax*4]
LDraw7:
 cmp bp,ds:word ptr[2+ecx]
 jl Lp2
 xor eax,eax
 mov ah,dh
 mov al,ds:byte ptr[esi]
 mov ds:word ptr[2+ecx],bp
LPatch7:
 mov al,ds:byte ptr[12345678h+eax]
 mov ds:byte ptr[1+edi],al
Lp2:
 add edx,ds:dword ptr[tstep]
 sbb eax,eax
 add ebp,ds:dword ptr[lzistepx]
 adc ebp,0
 add ebx,ds:dword ptr[_a_sstepxfrac]
 adc esi,ds:dword ptr[advancetable+4+eax*4]
LDraw6:
 cmp bp,ds:word ptr[4+ecx]
 jl Lp3
 xor eax,eax
 mov ah,dh
 mov al,ds:byte ptr[esi]
 mov ds:word ptr[4+ecx],bp
LPatch6:
 mov al,ds:byte ptr[12345678h+eax]
 mov ds:byte ptr[2+edi],al
Lp3:
 add edx,ds:dword ptr[tstep]
 sbb eax,eax
 add ebp,ds:dword ptr[lzistepx]
 adc ebp,0
 add ebx,ds:dword ptr[_a_sstepxfrac]
 adc esi,ds:dword ptr[advancetable+4+eax*4]
LDraw5:
 cmp bp,ds:word ptr[6+ecx]
 jl Lp4
 xor eax,eax
 mov ah,dh
 mov al,ds:byte ptr[esi]
 mov ds:word ptr[6+ecx],bp
LPatch5:
 mov al,ds:byte ptr[12345678h+eax]
 mov ds:byte ptr[3+edi],al
Lp4:
 add edx,ds:dword ptr[tstep]
 sbb eax,eax
 add ebp,ds:dword ptr[lzistepx]
 adc ebp,0
 add ebx,ds:dword ptr[_a_sstepxfrac]
 adc esi,ds:dword ptr[advancetable+4+eax*4]
LDraw4:
 cmp bp,ds:word ptr[8+ecx]
 jl Lp5
 xor eax,eax
 mov ah,dh
 mov al,ds:byte ptr[esi]
 mov ds:word ptr[8+ecx],bp
LPatch4:
 mov al,ds:byte ptr[12345678h+eax]
 mov ds:byte ptr[4+edi],al
Lp5:
 add edx,ds:dword ptr[tstep]
 sbb eax,eax
 add ebp,ds:dword ptr[lzistepx]
 adc ebp,0
 add ebx,ds:dword ptr[_a_sstepxfrac]
 adc esi,ds:dword ptr[advancetable+4+eax*4]
LDraw3:
 cmp bp,ds:word ptr[10+ecx]
 jl Lp6
 xor eax,eax
 mov ah,dh
 mov al,ds:byte ptr[esi]
 mov ds:word ptr[10+ecx],bp
LPatch3:
 mov al,ds:byte ptr[12345678h+eax]
 mov ds:byte ptr[5+edi],al
Lp6:
 add edx,ds:dword ptr[tstep]
 sbb eax,eax
 add ebp,ds:dword ptr[lzistepx]
 adc ebp,0
 add ebx,ds:dword ptr[_a_sstepxfrac]
 adc esi,ds:dword ptr[advancetable+4+eax*4]
LDraw2:
 cmp bp,ds:word ptr[12+ecx]
 jl Lp7
 xor eax,eax
 mov ah,dh
 mov al,ds:byte ptr[esi]
 mov ds:word ptr[12+ecx],bp
LPatch2:
 mov al,ds:byte ptr[12345678h+eax]
 mov ds:byte ptr[6+edi],al
Lp7:
 add edx,ds:dword ptr[tstep]
 sbb eax,eax
 add ebp,ds:dword ptr[lzistepx]
 adc ebp,0
 add ebx,ds:dword ptr[_a_sstepxfrac]
 adc esi,ds:dword ptr[advancetable+4+eax*4]
LDraw1:
 cmp bp,ds:word ptr[14+ecx]
 jl Lp8
 xor eax,eax
 mov ah,dh
 mov al,ds:byte ptr[esi]
 mov ds:word ptr[14+ecx],bp
LPatch1:
 mov al,ds:byte ptr[12345678h+eax]
 mov ds:byte ptr[7+edi],al
Lp8:
 add edx,ds:dword ptr[tstep]
 sbb eax,eax
 add ebp,ds:dword ptr[lzistepx]
 adc ebp,0
 add ebx,ds:dword ptr[_a_sstepxfrac]
 adc esi,ds:dword ptr[advancetable+4+eax*4]
 add edi,8
 add ecx,16
 dec bx
 jnz LDrawLoop
 pop esi
LNextSpan:
 add esi,32
LNextSpanESISet:
 mov edx,ds:dword ptr[8+esi]
 cmp edx,offset -999999
 jnz LSpanLoop
 pop edi
 pop ebp
 pop ebx
 pop esi
 ret
LExactlyOneLong:
 mov ecx,ds:dword ptr[4+esi]
 mov ebp,ds:dword ptr[28+esi]
 ror ebp,16
 mov ebx,ds:dword ptr[12+esi]
 cmp bp,ds:word ptr[ecx]
 jl LNextSpan
 xor eax,eax
 mov edi,ds:dword ptr[0+esi]
 mov ah,ds:byte ptr[24+1+esi]
 add esi,32
 mov al,ds:byte ptr[ebx]
 mov ds:word ptr[ecx],bp
LPatch9:
 mov al,ds:byte ptr[12345678h+eax]
 mov ds:byte ptr[edi],al
 jmp LNextSpanESISet
 public _D_PolysetAff8End
_D_PolysetAff8End:
 public _D_Aff8Patch
_D_Aff8Patch:
 mov eax, ds:[esp+4]
 mov dword ptr ds:LPatch1+2, eax
 mov dword ptr ds:LPatch2+2, eax
 mov dword ptr ds:LPatch3+2, eax
 mov dword ptr ds:LPatch4+2, eax
 mov dword ptr ds:LPatch5+2, eax
 mov dword ptr ds:LPatch6+2, eax
 mov dword ptr ds:LPatch7+2, eax
 mov dword ptr ds:LPatch8+2, eax
 mov dword ptr ds:LPatch9+2, eax
 ret
 public _D_PolysetDraw
_D_PolysetDraw:
 sub esp, 0C860h
 mov eax, esp
 add eax, 31
 and eax, 0FFFFFFE0h
 mov _a_spans, eax
 mov eax, [_r_affinetridesc+28]
 test eax, eax
 jz _D_DrawNonSubdiv
 push ebp
 mov ebp, [_r_affinetridesc+24]
 push esi
 shl ebp, 4
 push ebx
 mov ebx, [_r_affinetridesc+16]
 push edi
 mov edi, [_r_affinetridesc+20]
Llooptop:
 mov ecx, ds:[4-16+0+ebx+ebp]
 mov esi, ds:[4-16+4+ebx+ebp]
 shl ecx, 5
 mov edx, ds:[4-16+8+ebx+ebp]
 shl esi, 5
 add ecx, edi
 shl edx, 5
 add esi, edi
 add edx, edi
 fild dword ptr[0+4+ecx]
 fild dword ptr[0+4+esi]
 fild dword ptr[0+0+ecx]
 fild dword ptr[0+0+edx]
 fxch st(2)
 fsubr st(0), st(3)
 fild dword ptr[0+0+esi]
 fxch st(2)
 fsubr st(3), st(0)
 fild dword ptr[0+4+edx]
 fxch st(1)
 fsubrp st(3), st(0)
 fxch st(1)
 fmulp st(3), st(0)
 fsubp st(3), st(0)
 mov eax, [0+16+ecx]
 and eax, 0FF00h
 fmulp st(2), st(0)
 add eax, _acolormap
 fsubrp st(1), st(0)
 mov _d_pcolormap, eax
 fstp Ltemp
 mov eax, Ltemp
 sub eax, 080000001h
 jb short Lskip
 mov eax, ds:[0-16+ebx+ebp]
 test eax, eax
 jz short Lfacesback
 push edx
 push esi
 push ecx
 call near ptr _D_PolysetRecursiveTriangle
 sub ebp, 16
 jnz short Llooptop
 jmp short Ldone2
Lfacesback:
 mov eax, [0+8+ecx]
 push eax
 mov eax, [0+8+esi]
 push eax
 mov eax, [0+8+edx]
 push eax
 push ecx
 push edx
 mov eax, [_r_affinetridesc+32]
 test dword ptr[24+ecx], 00020h
 jz short Lp11
 add [0+8+ecx], eax
Lp11:
 test dword ptr[24+esi], 00020h
 jz short Lp12
 add [0+8+esi], eax
Lp12:
 test dword ptr[24+edx], 00020h
 jz short Lp13
 add [0+8+edx], eax
Lp13:
 push edx
 push esi
 push ecx
 call near ptr _D_PolysetRecursiveTriangle 
 pop edx
 pop ecx
 pop eax
 mov [0+8+edx], eax
 pop eax
 mov [0+8+esi], eax
 pop eax
 mov [0+8+ecx], eax
Lskip:
 sub ebp, 16
 jnz Llooptop
Ldone2:
 pop edi
 pop ebx
 pop esi
 pop ebp
 add esp, 0C860h
 ret
 public _D_PolysetScanLeftEdge
_D_PolysetScanLeftEdge:
 push ebp
 push esi
 push edi
 push ebx
 mov eax, ds:[esp+20]
 mov ecx, _d_sfrac
 and eax, 0FFFFh
 mov ebx, _d_ptex
 or ecx, eax
 mov esi, _d_pedgespanpackage
 mov edx, _d_tfrac
 mov edi, _d_light
 mov ebp, _d_zi
LScanLoop:
 mov [esi+12], ebx
 mov eax, _d_pdest
 mov [esi], eax
 mov eax, _d_pz
 mov [esi+4], eax
 mov eax, _d_aspancount
 mov [esi+8], eax
 mov [esi+24], edi
 mov [esi+28], ebp
 mov [esi+16], ecx
 mov [esi+20], edx
 mov al, [esi+32]
 add esi, 32
 mov eax, _erroradjustup
 mov _d_pedgespanpackage, esi
 mov esi, _errorterm
 add esi, eax
 mov eax, _d_pdest
 js short LNoLeftEdgeTurnover
 sub esi, _erroradjustdown
 add eax, _d_pdestextrastep
 mov _errorterm, esi
 mov _d_pdest, eax
 mov eax, _d_pz
 mov esi, _d_aspancount
 add eax, _d_pzextrastep
 add ecx, _d_sfracextrastep
 adc ebx, _d_ptexextrastep
 add esi, _d_countextrastep
 mov _d_pz, eax
 mov eax, _d_tfracextrastep
 mov _d_aspancount, esi
 add edx, eax
 jnb short LSkip1
 add ebx, [_r_affinetridesc+8]
LSkip1:
 add edi, _d_lightextrastep
 add ebp, _d_ziextrastep
 mov esi, _d_pedgespanpackage
 dec ecx
 test ecx, 0FFFFh
 jnz LScanLoop
 pop ebx
 pop edi
 pop esi
 pop ebp
 ret
LNoLeftEdgeTurnover:
 mov _errorterm, esi
 add eax, _d_pdestbasestep
 mov _d_pdest, eax
 mov eax, _d_pz
 mov esi, _d_aspancount
 add eax, _d_pzbasestep
 add ecx, dword ptr _d_sfracbasestep
 adc ebx, dword ptr _d_ptexbasestep
 add esi, _ubasestep
 mov _d_pz, eax
 mov _d_aspancount, esi
 mov esi, _d_tfracbasestep
 add edx, esi
 jnb short LSkip2
 add ebx, [_r_affinetridesc+8]
LSkip2:
 add edi, _d_lightbasestep
 add ebp, _d_zibasestep
 mov esi, _d_pedgespanpackage
 dec ecx
 test ecx, 0FFFFh
 jnz LScanLoop
 pop ebx
 pop edi
 pop esi
 pop ebp
 ret
; public _D_PolysetDrawFinalVerts
;_D_PolysetDrawFinalVerts:
; push ebp
; push ebx
; mov ecx,ds:dword ptr[8+8+esp]
; mov ebx,ds:dword ptr[4+8+esp]
; push esi
; push edi
;LFVLoop:
; mov eax,ds:dword ptr[0+0+ebx]
; mov edx,ds:dword ptr[_r_refdef+40]
; cmp eax,edx
; jge LNextVert
; mov esi,ds:dword ptr[0+4+ebx]
; mov edx,ds:dword ptr[_r_refdef+44]
; cmp esi,edx
; jge LNextVert
; mov edi,ds:dword ptr[_zspantable+esi*4]
; mov edx,ds:dword ptr[0+20+ebx]
; shr edx,16
; cmp dx,ds:word ptr[edi+eax*2]
; jl LNextVert
; mov ds:word ptr[edi+eax*2],dx
; mov edi,ds:dword ptr[0+12+ebx]
; shr edi,16
; mov edi,ds:dword ptr[_skintable+edi*4]
; mov edx,ds:dword ptr[0+8+ebx]
; shr edx,16
; mov dl,ds:byte ptr[edi+edx]
; mov edi,ds:dword ptr[0+16+ebx]
; and edi,0FF00h
; and edx,000FFh
; add edi,edx
; mov edx,ds:dword ptr[_acolormap]
; mov dl,ds:byte ptr[edx+edi*1]
; mov edi,ds:dword ptr[_d_scantable+esi*4]
; mov esi,ds:dword ptr[_d_viewbuffer]
; add edi,eax
; mov ds:byte ptr[esi+edi],dl
;LNextVert:
; add ebx,32
; dec ecx
; jnz LFVLoop
; pop edi
; pop esi
; pop ebx
; pop ebp
; ret
 public _D_DrawNonSubdiv
_D_DrawNonSubdiv:
 push ebp
 mov ebp, [_r_affinetridesc+24]
 push ebx
 shl ebp, 4
 push esi
 mov esi, [_r_affinetridesc+16]
 push edi
LNDLoop:
 mov edi, [_r_affinetridesc+20]
 mov ecx, [4+0-16+esi+ebp*1]
 shl ecx, 5
 mov edx, [4+4-16+esi+ebp*1]
 shl edx, 5
 mov ebx, [4+8-16+esi+ebp*1]
 shl ebx, 5
 add ecx, edi
 add edx, edi
 add ebx, edi
 mov eax, [0+4+ecx]
 mov esi, [0+0+ecx]
 sub eax, [0+4+edx]
 sub esi, [0+0+ebx]
 imul eax, esi
 mov esi, [0+0+ecx]
 mov edi, [0+4+ecx]
 sub esi, [0+0+edx]
 sub edi, [0+4+ebx]
 imul edi, esi
 sub eax, edi
 jns LNextTri
 mov _d_xdenom, eax
 fild _d_xdenom
 mov eax, [0+0+ecx]
 mov esi, [0+4+ecx]
 mov [_r_p0+0], eax
 mov [_r_p0+4], esi
 mov eax, [ecx+8]
 mov esi, [ecx+12]
 mov [_r_p0+8], eax
 mov [_r_p0+12], esi
 mov eax, [ecx+16]
 mov esi, [ecx+20]
 mov [_r_p0+16], eax
 mov [_r_p0+20], esi
 fdivr float_1
 mov eax, [edx+0]
 mov esi, [edx+4]
 mov [_r_p1+0], eax
 mov [_r_p1+4], esi
 mov eax, [edx + 8]
 mov esi, [edx + 12]
 mov [_r_p1 + 8], eax
 mov [_r_p1 + 12], esi
 mov eax, [edx + 16]
 mov esi, [edx + 20]
 mov [_r_p1 + 16], eax
 mov [_r_p1 + 20], esi
 mov eax, [ebx + 0]
 mov esi, [ebx + 4]
 mov [_r_p2 + 0], eax
 mov [_r_p2 + 4], esi
 mov eax, [ebx + 8]
 mov esi, [ebx + 12]
 mov [_r_p2 + 8], eax
 mov [_r_p2 + 12], esi
 mov eax, [ebx + 16]
 mov esi, [ebx + 20]
 mov [_r_p2 + 16], eax
 mov edi, [_r_affinetridesc + 16]
 mov [_r_p2 + 20], esi
 mov eax, [edi + ebp - 16]
 test eax, eax
 jnz short LFacesFront
 mov eax, [ecx + 24]
 mov esi, [edx + 24]
 mov edi, [ebx + 24]
 test eax, 00020h
 mov eax, [_r_affinetridesc + 32]
 jz short LOnseamDone0
 add [_r_p0 + 8], eax
LOnseamDone0:
 test esi, 00020h
 jz short LOnseamDone1
 add [_r_p1 + 8], eax
LOnseamDone1:
 test edi, 00020h
 jz short LOnseamDone2
 add [_r_p2 + 8], eax
LOnseamDone2:
LFacesFront:
 fstp _d_xdenom
 call near ptr _D_PolysetSetEdgeTable
 call near ptr _D_RasterizeAliasPolySmooth
LNextTri:
 mov esi, [_r_affinetridesc + 16]
 sub ebp, 16
 jnz LNDLoop
 pop edi
 pop esi
 pop ebx
 pop ebp
 add esp, 0C860h
 ret
_TEXT ENDS
 END