#pragma once

#include"__UtilShared.h"

#include<regex>

using namespace std;

namespace LingCommon {

	namespace Util {

		namespace ExtStd {



			template<class PointerType>
			class DereferenceLess {
			public:
				bool operator()(const PointerType& lhs, const PointerType& rhs) const;
			};
			template<class PointerType>
			bool DereferenceLess<PointerType>::operator()(const PointerType& lhs, const PointerType& rhs) const {

				return (*lhs) < (*rhs);
			}

			template<class Elem>
			class DefaultLooper {
			public:
				void loop(Elem& elem);
			};
			template<class Elem>
			void DefaultLooper<Elem>::loop(Elem& elem) {
				//do nothing
			}

			/// <summary>
			/// Delete instead of Delete Array
			/// </summary>
			/// <typeparam name="Elem"></typeparam>
			template<class Elem>
			class SafeDeleteLooper {
			public:
				void loop(Elem& elem);
			};
			template<class Elem>
			void SafeDeleteLooper<Elem>::loop(Elem& elem) {
				sdel(elem);
			}

			template<class Elem, class Less, class Looper = DefaultLooper<Elem>>
			class Ext_Vector {
			private:
			protected:
				bool isGiveUpInnerVec = false;
				std::vector<Elem>* p_innerVec;
				Less less;
				Looper looper;
			public:
				Ext_Vector();
				bool operator<(const Ext_Vector& rhs) const;
				std::vector<Elem>* giveUpInnerVector() const;
				std::vector<Elem>* getInnerVector() const;
				void loop();
				~Ext_Vector();
			};
			template<class Elem, class Less, class SafeDeleteLooper>
			Ext_Vector<Elem, Less, SafeDeleteLooper>::Ext_Vector() {
				this->p_innerVec = new std::vector<Elem>();
			}
			template<class Elem, class Less, class SafeDeleteLooper>
			bool Ext_Vector<Elem, Less, SafeDeleteLooper>::operator<(const Ext_Vector& rhs) const {
				//�ȱȽϳ���
				u64 length = rhs.p_innerVec->size();
				if (this->p_innerVec->size() > length) {
					return false;
				}
				if (this->p_innerVec->size() < length) {
					return true;
				}
				//��������ǳ������
				//����Ƚ�
				for (u64 i = 0; i < length; i++) {
					if (this->less(this->p_innerVec->at(i), rhs.p_innerVec->at(i)) == true) {
						//����true
						return true;
					}
					//����һ��������Լ�С��elem
					//˵��������Լ�С�����ؼ�
					if (this->less(rhs.p_innerVec->at(i), this->p_innerVec->at(i)) == true) {
						return false;
					}
				}

				//���е�����˵���Լ�����rhs�����ؼ�
				return false;
			}
			template<class Elem, class Less, class SafeDeleteLooper>
			std::vector<Elem>* Ext_Vector<Elem, Less, SafeDeleteLooper>::giveUpInnerVector() const {
				this->isGiveUpInnerVec = true;
				return this->p_innerVec;
			}
			template<class Elem, class Less, class SafeDeleteLooper>
			std::vector<Elem>* Ext_Vector<Elem, Less, SafeDeleteLooper>::getInnerVector() const {
				return this->p_innerVec;
			}
			template<class Elem, class Less, class SafeDeleteLooper>
			void Ext_Vector<Elem, Less, SafeDeleteLooper>::loop() {
				//ѭ������Elem
				u64 length = this->p_innerVec->size();
				for (u64 i = 0; i < length; i++) {
					looper.loop(this->p_innerVec->at(i));
				}
			}
			template<class Elem, class Less, class SafeDeleteLooper>
			Ext_Vector<Elem, Less, SafeDeleteLooper>::~Ext_Vector() {
				if (this->isGiveUpInnerVec == false) {
					del(this->p_innerVec);
				}
			}
		}
	}
}