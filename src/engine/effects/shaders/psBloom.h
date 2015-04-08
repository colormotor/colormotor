#pragma once

static const char * psBloom = STRINGIFY(

varying vec4 color;

uniform sampler2D inTex;
uniform sampler2D rampTex;

uniform float blurAmt;
uniform float sumAmt;

void main()
{
   vec4 sum = vec4(0);
   vec2 texcoord = vec2(gl_TexCoord[0]);
   int j;
   int i;

   for( i= -4 ;i < 4; i++)
   {
        for (j = -3; j < 3; j++)
        {
            sum += texture2D(inTex, texcoord + vec2(j, i)*blurAmt) * 0.25;
        }
   }
   
   gl_FragColor = sum*sum*sumAmt + texture2D(inTex, texcoord);
   
   /*
       if (texture2D(inTex, texcoord).r < 0.3)
    {
       gl_FragColor = sum*sum*0.012 + texture2D(inTex, texcoord);
    }
    else
    {
        if (texture2D(inTex, texcoord).r < 0.5)
        {
            gl_FragColor = sum*sum*0.009 + texture2D(inTex, texcoord);
        }
        else
        {
            gl_FragColor = sum*sum*0.0075 + texture2D(inTex, texcoord);
        }
    }*/
}

);