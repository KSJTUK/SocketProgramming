#pragma once

//���� Ŭ����
class DrawBuffer;

class Shape abstract {
public:
	explicit Shape(std::shared_ptr<DrawBuffer> drawBuffer);
	virtual ~Shape();

private:
	std::shared_ptr<DrawBuffer> mDrawBuffer{ };
};

