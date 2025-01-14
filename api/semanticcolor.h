/*****************************************************************************\
 *                        ANALYSIS PERFORMANCE TOOLS                         *
 *                               libparaver-api                              *
 *                      API Library for libparaver-kernel                    *
 *****************************************************************************
 *     ___     This library is free software; you can redistribute it and/or *
 *    /  __         modify it under the terms of the GNU LGPL as published   *
 *   /  /  _____    by the Free Software Foundation; either version 2.1      *
 *  /  /  /     \   of the License, or (at your option) any later version.   *
 * (  (  ( B S C )                                                           *
 *  \  \  \_____/   This library is distributed in hope that it will be      *
 *   \  \__         useful but WITHOUT ANY WARRANTY; without even the        *
 *    \___          implied warranty of MERCHANTABILITY or FITNESS FOR A     *
 *                  PARTICULAR PURPOSE. See the GNU LGPL for more details.   *
 *                                                                           *
 * You should have received a copy of the GNU Lesser General Public License  *
 * along with this library; if not, write to the Free Software Foundation,   *
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA          *
 * The GNU LEsser General Public License is contained in the file COPYING.   *
 *                                 ---------                                 *
 *   Barcelona Supercomputing Center - Centro Nacional de Supercomputacion   *
\*****************************************************************************/


#pragma once

#include <functional>
#include <map>
#include "paravertypes.h"
#include "paraverkerneltypes.h"

class Timeline;

enum class TColorFunction
{
  COLOR = 0,
  GRADIENT,
  NOT_NULL_GRADIENT,
  FUNCTION_LINE,
  PUNCTUAL,
  FUSED_LINES,
  ALTERNATIVE_GRADIENT
};

class SemanticColor
{
  public:
    static rgb BACKGROUND;
    static rgb FOREGROUND;
    static rgb ZERO_AXIS;

    static rgb DEFAULT_PUNCTUAL;
    static rgb DEFAULT_LOGICAL_COMMUNICATIONS;
    static rgb DEFAULT_PHYSICAL_COMMUNICATIONS;
    static rgb DEFAULT_BEGIN_GRADIENT_COLOR;
    static rgb DEFAULT_END_GRADIENT_COLOR;
    static rgb DEFAULT_NEGATIVE_BEGIN_GRADIENT_COLOR;
    static rgb DEFAULT_NEGATIVE_END_GRADIENT_COLOR;
    static rgb DEFAULT_ABOVE_OUTLIER_COLOR;
    static rgb DEFAULT_BELOW_OUTLIER_COLOR;

    static PRV_UINT32 getNumColors();
    static rgb* getCodeColors();

    static rgb getBeginGradientColor();
    static rgb getEndGradientColor();
    static rgb getNegativeBeginGradientColor();
    static rgb getNegativeEndGradientColor();
    static rgb getAboveOutlierColor();
    static rgb getBelowOutlierColor();

    template<typename C>
    static int getLuminance( C red, C green, C blue )
    {
      return ( red   * 30 ) / 100 +
             ( green * 59 ) / 100 +
             ( blue  * 11 ) / 100;
    }

    virtual ~SemanticColor()
    {};

    virtual rgb calcColor( TSemanticValue whichValue,
                           TSemanticValue minimum,
                           TSemanticValue maximum,
                           bool useCustomPalette ) const = 0;

  private:
    static PRV_UINT32 numColors;
    static rgb codeColor[];

    static rgb beginGradientColor;
    static rgb endGradientColor;
    static rgb negativeBeginGradientColor;
    static rgb negativeEndGradientColor;
    static rgb aboveOutlierColor;
    static rgb belowOutlierColor;
};

class CodeColor: public SemanticColor
{
  public:
    CodeColor();
    CodeColor( Timeline *dummy )
    {}
    ~CodeColor();

    PRV_UINT32 getNumColors() const;
    void setColor( PRV_UINT32 pos, rgb color );
    void setCustomColor( TSemanticValue whichValue, rgb color );
    bool existCustomColors() const;
    const std::map<TSemanticValue, rgb>& getCustomPalette() const;
    void setCustomPalette( const std::map<TSemanticValue, rgb>& whichPalette );
    rgb calcColor( TSemanticValue whichValue,
                   TSemanticValue minimum,
                   TSemanticValue maximum,
                   bool useCustomPalette ) const override;

  private:
    std::vector<rgb> colors;
    std::map<TSemanticValue, rgb> customPalette;

    inline rgb getColor( PRV_UINT32 pos ) const;
    void expandColors();

    bool isColorSimilarToBackground( rgb whichColor ) const;

    static const int MAX_COLORS;
};


enum class TGradientFunction
{
  LINEAR = 0,
  STEPS,
  LOGARITHMIC,
  EXPONENTIAL
};

class GradientColor: public SemanticColor
{
  public:
    using TRangeFunctions = std::map< double, std::function< rgb( TSemanticValue )> >;

    GradientColor();
    GradientColor(  const std::vector< rgb >& whichStopColors );
    ~GradientColor();

    void setBeginGradientColor( rgb color );
    rgb getBeginGradientColor() const;

    void setEndGradientColor( rgb color );
    rgb getEndGradientColor() const;

    void setNegativeBeginGradientColor( rgb color );
    rgb getNegativeBeginGradientColor() const;

    void setNegativeEndGradientColor( rgb color );
    rgb getNegativeEndGradientColor() const;

    void setAboveOutlierColor( rgb color );
    rgb getAboveOutlierColor() const;

    void setBelowOutlierColor( rgb color );
    rgb getBelowOutlierColor() const;

    void allowOutliers( bool activate );
    bool getAllowOutliers() const;
    void allowOutOfScale( bool activate );
    bool getAllowOutOfScale() const;

    TGradientFunction getGradientFunction() const;
    void setGradientFunction( TGradientFunction whichFunction );

    rgb calcColor( TSemanticValue whichValue,
                   TSemanticValue minimum,
                   TSemanticValue maximum,
                   bool useCustomPalette = false ) const override;
    bool isColorOutlier( rgb whichColor ) const;

    void copy( GradientColor &destiny );

  private:
    bool drawOutlier;
    bool drawOutOfScale;
    
    TRangeFunctions rangeFunctions;
    TRangeFunctions negativeRangeFunctions;
    std::vector< rgb > stopColors;
    std::vector< rgb > negativeStopColors;
    rgb aboveOutlierColor;
    rgb belowOutlierColor;

    double redStep;
    double greenStep;
    double blueStep;

    double negativeRedStep;
    double negativeGreenStep;
    double negativeBlueStep;

    TGradientFunction function;

    void initCommon();
    void recalcSteps();
};


class Normalizer
{
  public:
    static double calculate( TSemanticValue whichValue,
                             TSemanticValue whichMinimum,
                             TSemanticValue whichMaximum,
                             TGradientFunction whichFunction,
                             bool minimumAsBase );
  private:
    static PRV_INT16 numSteps;

};



