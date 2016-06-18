<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:template match="/">

  <html>
  <body>
    <center>
      <h2>Rendez Vous Server Tables</h2>
      <xsl:for-each select="simulation/Notification/Network/Node/RendezVousServer">
              <h3>Table for node <xsl:value-of select="../@id" /> at <xsl:value-of select="../../../time" /></h3>
                
                <table border="1" cellspacing="0">    
                    <tr bgcolor="#808080">
                      <th>Node</th>
                      <th>Primary Address</th>
                    </tr>
              
                <xsl:for-each select="Entry">
                    <tr>
                    <td><xsl:value-of select="node" /></td>
                    <td><xsl:value-of select="address" /></td>
                    </tr>
                </xsl:for-each>
                
                </table>
      </xsl:for-each>
   </center>        
  </body>
  </html>
</xsl:template>
</xsl:stylesheet>