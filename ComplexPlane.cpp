#include "ComplexPlane.h"

ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight)
    : m_pixel_size(pixelWidth, pixelHeight),
    m_aspectRatio (pixelHeight / pixelWidth),
    m_plane_size(BASE_WIDTH, BASE_HEIGHT * m_aspectRatio),
    m_zoomCount(0),
    m_state(State::CALCULATING),
    m_vArray(Points, pixelWidth * pixelHeight)
{
}

void ComplexPlane::draw(RenderTarget& target, RenderStates states) const
{
	target.draw(m_vArray);
}

void ComplexPlane::updateRender()
{
	if (m_state == State::CALCULATING)
		for (int j = 0; j < m_pixel_size.x; j++)
		{
			for (int i = 0; i < m_pixel_size.y; i++)
			{
				Vector2f complexCoord = mapPixelToCoords(Vector2i(j, i));
				int iterations = countIterations(complexCoord);
				Uint8 r, g, b;
				iterationsToRGB(iterations, r, g, b);
				m_vArray[j + i * m_pixel_size.x].color = { r, g, b };
			}
			m_state = State::DISPLAYING;
		}

}

void ComplexPlane::zoomIn()
{
	m_zoomCount++;

	float x = pow(BASE_WIDTH * BASE_ZOOM, m_zoomCount);

	float y = ((BASE_HEIGHT * m_aspectRatio) * pow(BASE_ZOOM, m_zoomCount));

	m_plane_size = { x, y };
	m_state = State::CALCULATING;

}

void ComplexPlane::zoomOut()
{
	m_zoomCount--;

	float x = pow(BASE_WIDTH * BASE_ZOOM, m_zoomCount);

	float y = ((BASE_HEIGHT * m_aspectRatio) * pow(BASE_ZOOM, m_zoomCount));

	m_plane_size = { x, y };
	m_state = State::CALCULATING;

}

void ComplexPlane::setCenter(Vector2i mousePixel)
{
	m_plane_center = mapPixelToCoords(mousePixel);

	m_state = State::CALCULATING;

}

void ComplexPlane::setMouseLocation(Vector2i mousePixel)
{
	m_mouseLocation = mapPixelToCoords(mousePixel);
}

void ComplexPlane::loadText(Text& text)
{
	stringstream gremlin;

	gremlin << "Mandelbrot Set" << endl;
	gremlin << "Center: " << m_plane_center.x << endl;
	gremlin << "Curser: (" << m_mouseLocation.x << "," << m_mouseLocation.y << ")" << endl;
	gremlin << "Left_click to Zoom out" << endl;
	gremlin << "Right-click to Zoom out" << endl;

}

int ComplexPlane::countIterations(Vector2f coord)
{
	size_t maxIterations = MAX_ITER;
	size_t iterations = 0;
	std::complex<float> z(coord.x, coord.y);

	while (iterations < maxIterations && std::abs(z) < 2.0) {
		z = z * z + std::complex<float>(coord.x, coord.y);
		iterations++;
	}

	return iterations;
}

void ComplexPlane::iterationsToRGB(size_t count, Uint8& r, Uint8& g, Uint8& b)
{
	r = (255 * (count) / (MAX_ITER)); 
	g = (255 * (count) / (MAX_ITER)); 
	b = (255 * (count) / (MAX_ITER));
}

Vector2f ComplexPlane::mapPixelToCoords(Vector2i mousePixel)
{
	float realPart = ((mousePixel.x - 0) / (m_pixel_size.x)) * m_plane_size.x
		+ (m_plane_center.x - m_plane_size.x / 2.0f);

	float imagPart = (mousePixel.y - m_pixel_size.y) / (0 - m_pixel_size.y) * m_plane_size.y
		+ (m_plane_center.y - m_plane_size.y / 2.0f);

	return Vector2f(realPart, imagPart);

}
