<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:template match="/">

  <html>
  <body>
    <center>
      <h2>Trace Route</h2>
      <xsl:for-each select="simulation/Notification">
          <xsl:if test="@id='node.routing.trace'">
            
            <h3><xsl:value-of select="TraceRoute/@id" /></h3>
            
            <xsl:if test="TraceRoute/noRoute">
                <font color="#FF0000"><b>No Route to host</b></font>
            </xsl:if>
            <xsl:if test="TraceRoute/distance">
                Distance: <xsl:value-of select="TraceRoute/distance" />
             </xsl:if>

            <table border="1" cellspacing="0">    
                <tr bgcolor="#808080">
                  <th>Node</th>
                  <th>Address</th>
                </tr>
                 <xsl:for-each select="TraceRoute/hop">
                     <xsl:if test="returned">
                          <tr bgcolor="#F00000">
                            <td><xsl:value-of select="nodeName" /></td>
                            <td><xsl:value-of select="node" /></td>
                          </tr>
                     </xsl:if>
                     <xsl:if test="not(returned)">
                          <tr>
                            <td><xsl:value-of select="nodeName" /></td>
                            <td><xsl:value-of select="node" /></td>
                          </tr>

                     </xsl:if>
                 </xsl:for-each>

              </table>
              
          </xsl:if>
      </xsl:for-each>
    
      <h2>Routing Tables</h2>
      <xsl:for-each select="simulation/Notification/Network/Node/RoutingTable">

              <h3>Routing table for <xsl:value-of select="../@id" /> at <xsl:value-of select="../../../time" /></h3>
                <table border="1" cellspacing="0">    
                    <tr bgcolor="#808080">
                      <th>Destination</th>
                      <th>Next Hop</th>
                      <th>Distance</th>
                      <th>Visited Bitmap</th>
                    </tr>
              
                <xsl:for-each select="Entry">
                    <tr>
                    <td><xsl:value-of select="Destination" /></td>
                    <td><xsl:value-of select="NextHop" /></td>
                    <td><xsl:value-of select="Distance" /></td>
                    <td><xsl:value-of select="VisitedBitmap" /></td>
                    </tr>
                </xsl:for-each>
                
                </table>
            
      
      </xsl:for-each>


   </center>
    
    
  </body>
  </html>
</xsl:template>
</xsl:stylesheet>