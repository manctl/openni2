#ifndef __XNLINKCONTROLENDPOINT_H__
#define __XNLINKCONTROLENDPOINT_H__

#include "ISyncIOConnection.h"
#include "XnLinkMsgEncoder.h"
#include "XnLinkResponseMsgParser.h"
#include "XnArray.h"
#include "XnLinkDefs.h"
#include "XnLinkProtoLibDefs.h"
#include <XnBitSet.h>
#include <OpenNI.h>

struct XnShiftToDepthConfig;

namespace xn
{

class IConnectionFactory;
struct BaseStreamProps;

class LinkControlEndpoint
{
public:
	static const XnUInt32 MUTEX_TIMEOUT;

	LinkControlEndpoint();
	virtual ~LinkControlEndpoint();

	XnStatus Init(XnUInt32 nMaxOutMsgSize, IConnectionFactory* pConnectionFactory);
	void Shutdown();

	XnStatus Connect();
	void Disconnect();
	XnBool IsConnected() const;

#ifdef DATA_ON_CONTROL
	void SetStreamID(XnUInt16 nStreamID);
#endif

	//nResponseSize is max size on input, actual size on output
	//pIsLast - optional. If provided, command will not automatically continue response, and the out value is whether this is the last packet. If NULL, all data is fetched automatically.
	XnStatus ExecuteCommand(XnUInt16 nMsgType, XnUInt16 nStreamID, const void* pCmdData, XnUInt32 nCmdSize, void* pResponseData, XnUInt32& nResponseSize, XnBool* pIsLast = NULL);
	XnStatus SendData(XnUInt16 nMsgType, const void* pCmdData, XnUInt32 nCmdSize, void* pResponseData, XnUInt32& nResponseSize);
	XnUInt16 GetPacketID() const;
	XN_MUTEX_HANDLE GetMutex() const;
	XnBool IsMsgTypeSupported(XnUInt16 nMsgType);
	XnBool IsPropertySupported(XnUInt16 nPropID);

	/* Specific commands */
	XnStatus GetFWVersion(XnLinkDetailedVersion& version);
	XnStatus GetProtocolVersion(XnLeanVersion& version);
	XnStatus GetHardwareVersion(XnUInt32& version);
	XnStatus GetSerialNumber(XnChar* strSerialNumber, XnUInt32 nSize);
	XnStatus GetComponentsVersions(xnl::Array<XnComponentVersion>& components);
	XnStatus GetSupportedMsgTypes(xnl::Array<xnl::BitSet>& supportedMsgTypes);
	XnStatus GetSupportedProperties(xnl::Array<xnl::BitSet>& supportedProperties);
	XnStatus GetSupportedInterfaces(XnUInt16 nStreamID, xnl::BitSet& supportedInterfaces);
	XnStatus GetBootStatus(XnBootStatus& bootStatus);
	XnStatus UploadFile(const XnChar* strFileName, XnBool bOverrideFactorySettings);
	XnStatus GetFileList(xnl::Array<XnFileEntry>& files);
	XnStatus DownloadFile(XnUInt16 zone, const XnChar* fwFileName, const XnChar* targetFile);
	XnStatus StartStreaming(XnUInt16 nStreamID);
	XnStatus StopStreaming(XnUInt16 nStreamID);
	XnStatus SoftReset();
	XnStatus HardReset();
	XnStatus GetSupportedBistTests(xnl::Array<XnBistTest>& supportedTests);
	XnStatus ExecuteBistTests(XnUInt32 nID, XnBistTestResponse* pResponse, XnUInt32 nResponseStructSize);
	XnStatus StartUsbTest();
	XnStatus StopUsbTest();
	XnStatus GetSupportedI2CDevices(xnl::Array<XnLinkI2CDevice>& supporteddevices);
	XnStatus WriteI2C(XnUInt8 nDeviceID, XnUInt8 nAddressSize, XnUInt32 nAddress, XnUInt8 nValueSize, XnUInt32 nValue, XnUInt32 nMask);
    XnStatus ReadI2C(XnUInt8 nDeviceID, XnUInt8 nAddressSize, XnUInt32 nAddress, XnUInt8 nValueSize, XnUInt32& nValue);
	XnStatus WriteAHB(XnUInt32 nAddress, XnUInt32 nValue, XnUInt8 nBitOffset, XnUInt8 nBitWidth);
	XnStatus ReadAHB(XnUInt32 nAddress, XnUInt8 nBitOffset, XnUInt8 nBitWidth, XnUInt32& nValue);
	XnStatus GetShiftToDepthConfig(XnUInt16 nStreamID, XnShiftToDepthConfig& shiftToDepthConfig);
	XnStatus SetVideoMode(XnUInt16 nStreamID, const XnStreamVideoMode& videoMode);
	XnStatus GetVideoMode(XnUInt16 nStreamID, XnStreamVideoMode& videoMode);
	XnStatus GetSupportedVideoModes(XnUInt16 nStreamID, xnl::Array<XnStreamVideoMode>& supportedVideoModes);
	XnStatus EnumerateStreams(xnl::Array<XnStreamInfo>& aStreamInfos);
    XnStatus CreateInputStream(XnStreamType streamType, const XnChar* strCreationInfo, XnUInt16& nStreamID, XnUInt16& nEndpointID);
	XnStatus DestroyInputStream(XnUInt16 nStreamID);
	XnStatus SetCropping(XnUInt16 nStreamID, const OniCropping& cropping);
	XnStatus GetCropping(XnUInt16 nStreamID, OniCropping& cropping);
    XnStatus SetEmitterActive(XnBool bActive);
	XnStatus GetSupportedLogFiles(xnl::Array<XnLinkLogFile>& supportedFiles);
	XnStatus OpenFWLogFile(XnUInt8 logID, XnUInt16 nLogStreamID);
	XnStatus CloseFWLogFile(XnUInt8 logID, XnUInt16 nLogStreamID);

    //TODO: Implement Get emitter active

    XnStatus GetStreamFragLevel(XnUInt16 nStreamID, XnStreamFragLevel& streamFragLevel);
	XnStatus GetMirror(XnUInt16 nStreamID, XnBool& bMirror);
    XnStatus SetMirror(XnUInt16 nStreamID, XnBool bMirror);
    
	XnStatus BeginUpload();
	XnStatus EndUpload();
	XnStatus FormatZone(XnUInt8 nZone);

	/*DepthGenerator commands */
	XnStatus GetCameraIntrinsics(XnUInt16 nStreamID, XnLinkCameraIntrinsics& cameraIntrinsics);

private:
	static const XnUInt16 BASE_PACKET_ID;
	static const XnUInt16 MAX_RESPONSE_NUM_PACKETS; //Max number of packets in response
	static const XnChar MUTEX_NAME[];

	XnStatus GetLogicalMaxPacketSize(XnUInt16& nMaxPacketSize);
	
	/** The fragmentation parameter in this function indicates the fragmentation of the whole data BLOCK, 
	    not an individual packet. So if it's BEGIN, it means this block of data (one or more packets) begins 
		the message. If it's MIDDLE, it's the middle of the message, and if it's END, this block ends the 
		message.**/
	XnStatus ExecuteImpl(XnUInt16 nMsgType, 
	                      XnUInt16 nStreamID,
	                      const void* pData, 
						  XnUInt32 nSize, 
						  XnLinkFragmentation fragmentation,
						  void* pResponseData, 
						  XnUInt32& nResponseSize,
						  XnBool autoContinue,
						  XnBool& isLast);

	XnStatus ContinueResponseImpl(XnUInt16 originalMsgType, XnUInt16 streamID, void* pResponseData, XnUInt32& nResponseSize, XnBool& outLastPacket);

	XnStatus ValidateResponsePacket(const LinkPacketHeader* pResponsePacket, 
	                                XnUInt16 nExpectedMsgType,
									XnUInt16 nExpectedStreamID,
									XnUInt32 nBytesToRead);

	/* Properties */
	XnStatus SetIntProperty(XnUInt16 nStreamID, XnLinkPropID propID, XnUInt64 nValue);
	XnStatus GetIntProperty(XnUInt16 nStreamID, XnLinkPropID propID, XnUInt64& nValue);
	XnStatus SetRealProperty(XnUInt16 nStreamID, XnLinkPropID propID, XnDouble dValue);
	XnStatus GetRealProperty(XnUInt16 nStreamID, XnLinkPropID propID, XnDouble& dValue);
	XnStatus SetStringProperty(XnUInt16 nStreamID, XnLinkPropID propID, const XnChar* strValue);
	XnStatus GetStringProperty(XnUInt16 nStreamID, XnLinkPropID propID, XnUInt32 nSize, XnChar* strValue);
	XnStatus SetGeneralProperty(XnUInt16 nStreamID, XnLinkPropID propID, XnUInt32 nSize, const void* pSource);
	//nSize is max size on input, actual size on output
	XnStatus GetGeneralProperty(XnUInt16 nStreamID, XnLinkPropID propID, XnUInt32& nSize, void* pDest);
	XnStatus GetBitSetProperty(XnUInt16 nStreamID, XnLinkPropID propID, xnl::BitSet& bitSet);
	XnStatus SetProperty(XnUInt16 nStreamID, XnLinkPropType propType, XnLinkPropID propID, XnUInt32 nSize, const void* pSource);
	XnStatus GetProperty(XnUInt16 nStreamID, XnLinkPropType propType, XnLinkPropID propID, XnUInt32& nSize, void* pDest);

	union
	{
		XnUInt8* m_pIncomingRawPacket; //Holds one packet, used for receiving from connection
		LinkPacketHeader* m_pIncomingPacket;
	};

	XnUInt32 m_nMaxOutMsgSize;
	ISyncIOConnection* m_pConnection;
	LinkMsgEncoder m_msgEncoder;
	LinkResponseMsgParser m_responseMsgParser;
	XnUInt8* m_pIncomingResponse; //Holds complete parsed response (without link headers)
    XnUInt32 m_nMaxResponseSize;
	XnBool m_bInitialized;
	XnBool m_bConnected;
	XnUInt16 m_nPacketID;
	XnUInt16 m_nMaxPacketSize;
	XN_MUTEX_HANDLE m_hMutex;
	xnl::Array<xnl::BitSet> m_supportedMsgTypes; //Array index is msgtype hi byte, position in bit set is msgtype lo byte.
};

}
#endif // __XNLINKCONTROLENDPOINT_H__
