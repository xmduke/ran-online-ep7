/**********************************************************************
 *<
	FILE: IDX9VertexShader.h

	DESCRIPTION: DirectX 9 Vertex Shader Interface Definition

	CREATED BY: Nikolai Snader and Norbert Jeske

	HISTORY: Created 9/22/00

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include <d3dx9.h>
#include "IHardwareShader.h"

#define DX9_VERTEX_SHADER_INTERFACE_ID Interface_ID(0x25065eae, 0x433c2537)

struct DX9VSConstant
{
	float a,b,c,d;
	
	// Access operators
	float& operator[](int i) { return (&a)[i]; }     
	const float& operator[](int i) const { return (&a)[i]; }  
};

class ID3D9GraphicsWindow;
class IDX9PixelShader;

class IDX9VertexShader : virtual public IVertexShader, public BaseInterface
{
public:
	virtual Interface_ID GetID() { return DX9_VERTEX_SHADER_INTERFACE_ID; }

	// Confirm that the Direct3D Device can handle this VertexShader
	virtual HRESULT ConfirmDevice(ID3D9GraphicsWindow *gw) = 0;

	// Confirm that an associated PixelShader will work with this VertexShader
	virtual HRESULT ConfirmPixelShader(IDX9PixelShader *pps) = 0;

	// Can try tristrips for drawing or must geometry using this VertexShader
	// be drawn as triangles?  This should return 'true' unless additional per
	// vertex data is generated by this VertexShader and this data does not map
	// to the Mesh vertices in the same way as existing data the Mesh knows
	// about such as texture coordinates.
	virtual bool CanTryStrips() = 0;

	// Number of passes for the effect this VertexShader creates.  Note that
	// this value will depend on the hardware currently in use.
	virtual int GetNumMultiPass() = 0;

	// Retrieve the VertexShader handle for the specified pass for use in GFX
	virtual LPDIRECT3DVSHADER9 GetVertexShaderHandle(int numPass) = 0;

	// Set the VertexShader for the specified pass.  This call will be made at
	// least once per object to set the per object data for the VertexShader
	// such as the VertexShader constants.
	virtual HRESULT SetVertexShader(ID3D9GraphicsWindow *gw, int numPass) = 0;


	// Drawing functions.  These functions are necessary as something other
	// than a simple default body if:
	//
	// 1. The VertexShader needs to add additional per vertex data unknown to
	//    the Mesh to the VertexBuffer.
	//
	// 2. The VertexShader needs to have per vertex data ordered differently
	//    than the standard position, normal, {color, tex coords ordering}.
	//
	// 3. The VertexShader is being used to create cached VertexBuffers or
	//    using higher order surfaces.
	//
	// In the first two cases, the VertexShader has the option of not only
	// locking and filling the VertexBuffer with data, but also of making the
	// actual DrawPrimitive call.  In the third case, the VertexShader must
	// make the DrawPrimitive call.  The VertexShader indicates that it has
	// done the drawing by returning 'true' in the Draw() functions below.
	//
	// In the case where the VertexShader does not want to do the drawing but
	// does want to fill in a VertexBuffer with data, the VertexShader can
	// request the GFX to create a VertexBuffer (and possibly an IndexBuffer)
	// of appropriate size.  The GetVertexBuffer and GetIndexBuffer calls on
	// the ID3D9GraphicsWindow object will do this and return the allocated
	// buffers in subsequent calls or reallocate them if necessary.
	//
	// Please note that if a PixelShader or PixelShaders are in use, these
	// Draw() functions may need to set them for the appropriate passes of a
	// multipass rendering if the drawing is done in these Draw() functions.
	// If the GFX is doing the drawing, then these Draw() functions are only
	// being used to fill in the VertexBuffer with data; the GFX will be doing
	// the drawing and will be setting the PixelShaders as appropriate.


	// Draw 3D Mesh as TriStrips.  Fill in the VertexBuffer with data in the
	// order desired by the VertexShader.  Return 'true' if the Mesh has
	// actually been drawn in this call, 'false' if the GFX is required to make
	// the DrawPrimitive call.
	virtual bool	DrawMeshStrips(ID3D9GraphicsWindow *gw, MeshData *data) = 0;

	// Draw 3D Mesh as wireframe.  Fill in the VertexBuffer with data in the
	// order desired by the VertexShader.  Return 'true' if the Mesh has
	// actually been drawn in this call, 'false' if the GFX is required to make
	// the DrawPrimitive call.
	virtual bool	DrawWireMesh(ID3D9GraphicsWindow *gw, WireMeshData *data) = 0;


	// Draw 3D lines.  A Mesh is being drawn by having line segments handed
	// down one at a time.

	// Pass in the Mesh data in preparation for drawing 3D lines.
	virtual void	StartLines(ID3D9GraphicsWindow *gw, WireMeshData *data) = 0;

	// Add the connectivity information for one two point line segment.
	virtual void	AddLine(ID3D9GraphicsWindow *gw, DWORD *vert, int vis) = 0;

	// Draw the line segments accumulated.  This should restart the filling of
	// a VertexBuffer with the next AddLine call if additional data needs to
	// be drawn before EndLines is called.  Return 'true' if the Mesh line
	// segments have actually been drawn in this call, 'false' if the GFX is
	// required to make the DrawPrimitive call.
	virtual bool	DrawLines(ID3D9GraphicsWindow *gw) = 0;

	// Let the Mesh know that all drawing and data access is finished.
	virtual void	EndLines(ID3D9GraphicsWindow *gw, GFX_ESCAPE_FN fn) = 0;


	// Draw 3D triangles.  A Mesh is being drawn by having triangles handed
	// down one at a time.

	// Pass in the Mesh data in preparation for drawing 3D triangles.
	virtual void	StartTriangles(ID3D9GraphicsWindow *gw, MeshFaceData *data) = 0;

	// Add the connectivity information for one triangle.
	virtual void	AddTriangle(ID3D9GraphicsWindow *gw, DWORD index, int *edgeVis) = 0;

	// Draw the triangles accumulated.  This should restart the filling of a
	// VertexBuffer with the next AddTriangle call if additional data needs to
	// be drawn before EndTriangles is called.  Return 'true' if the Mesh
	// triangles have actually been drawn in this call, 'false' if the GFX is
	// required to make the DrawPrimitive call.
	virtual bool	DrawTriangles(ID3D9GraphicsWindow *gw) = 0;

	// Let the Mesh know that all drawing and data access is finished.
	virtual void	EndTriangles(ID3D9GraphicsWindow *gw, GFX_ESCAPE_FN fn) = 0;
};
